#include "Arduino.h"
#include "SPI.h"
#include "RWM.h"


//This function will write a value to a register on the ADXL345.
//Parameters:
//  char registerAddress - The register to write a value to
//  char value - The value to be written to the specified register.
void RWMwriteRegister(char registerAddress, char value){
  //Set Chip Select pin low to signal the beginning of an SPI packet.
  digitalWrite(RWM_ADXL_CS, LOW);
  //Transfer the register address over SPI.
  SPI.transfer(registerAddress);
  //Transfer the desired register value over SPI.
  SPI.transfer(value);
  //Set the Chip Select pin high to signal the end of an SPI packet.
  digitalWrite(RWM_ADXL_CS, HIGH);
}

RWM::RWM() {
  pinMode(RWM_EEPROM_CS, OUTPUT); //initialize chip select pin for EEPROM, should this be INPUT??
  digitalWrite(RWM_EEPROM_CS, HIGH); // set EEPROM chip select pin to high
  pinMode(RWM_ADXL1_CS, OUTPUT); //initialize accelerometer chip select pin
  digitalWrite(RWM_ADXL1_CS, HIGH); // set accelerometer chip select pin to high
  pinMode(RWM_ADXL2_CS, OUPUT);
  digitalWrite(RWM_ADXL2_CS, HIGH);
  SPI.begin();
  SPI.setDataMode(SPI_MODE03) //see arduino spi page for more info, the EEPROM is in this data mode and the ADXL is set to be in this data mode in writeregister
  SPI.setClockDivider(SPI_CLOCK_DIV8); //this was in Brad's code, so I did it the same way...
  RWMwriteRegister(RWM_ADXL_DATA, 01000011); // write to the register of the ADXL to set the DATA_FORMAT, which includes some settings
  //see the datasheet (search DATA_FORMAT) for what this value should be, the second 1 sets to 3-wire mode,the last two 1s set the range to +/-16g
}

byte RWM::EEPROMreadStatus(void) {
  //figure out the status of values such as WIP (Write in Progress) or WREN (Write Enable), can be used to make sure EEPROM is ready to take information
  byte value;
  digitalWrite(RWM_EEPROM_CS, LOW);
  SPI.transfer(RWM_RDSR);
  value = SPI.transfer(0);
  digitalWrite(RWM_EEPROM_CS, HIGH);
  return value;
}

void RWM::EEPROMwriteEnable(void) {
  //enable the EEPROM to take information
  digitalWrite(RWM_EEPROM_CS, LOW);
  //digitalWrite(RWM_EEPROM_HOLD, HIGH) //supposedly this is held high by being wired to 5V, did we do this? if not, I'll go around and add it
  SPI.transfer(RWM_EEPROM_WREN);
  digitalWrite(RWM_EEPROM_CS, HIGH);

}

void RWM::EEPROMchipErase(void) {
  //erase the EEPROM, code taken directly from BRAD, may need to set HOLD pins
  digitalWrite(RWM_EEPROM_CS, LOW);
  //digitalWrite(RWM_EEPROM_HOLD, HIGH) // see comments in above function about setting the hold pin
  SPI.transfer(RWM_CE);
  //digitalWrite(RWM_EEPROM_HOLD, LOW)
  digitalWrite(RWM_EEPROM_CS, HIGH);
}

void RWM::EEPROMsleep(void) {

}


//This function will read all of the data from the accelerometer
void RWM::RWMReadAccelerometer(unsigned long address, values*){
  //Since we're performing a read operation, the most significant bit of the register address should be set.
  //char address = 0x80 | registerAddress;
  //If we're doing a multi-byte read, bit 6 needs to be set as well.
  //if(numBytes > 1)address = address | 0x40;
  //unsigned char values[6];
  //Set the Chip select pin low to start an SPI packet.
  digitalWrite(RWM_ADXL_CS, LOW);
  //Transfer the starting register address that needs to be read.
  //Continue to read registers until we've read the number specified, storing the results to the input buffer.
  SPI.transfer(DATAX0);
  values[0] = SPI.transfer(0);
  SPI.transfer(DATAX1);
  values[1] =SPI.transfer(0);
  SPI.transfer(DATAY0);
  values[2] = SPI.transfer(0);
  SPI.transfer(DATAY1); 
  values[3] = SPI.transfer(0);
  SPI.transfer(DATAZ0);
  values[4] = SPI.transfer(0);
  SPI.transfer(DATAZ1);
  values[5] = SPI.transfer(0);
  //Set the Chips Select pin high to end the SPI packet.
  digitalWrite(RWM_ADXL_CS, HIGH);
}

void RWM::EEPROMwriteByte(unsigned long address, byte value) {
  word temp;
  
  if (address<=0x1FFFF) {
    while (EEPROMreadStatus()&0x01) {}
    EEPROMwriteEnable();
    digitalWrite(RWM_EEPROM_CS, LOW);
    SPI.transfer(RWM_EEPROM_WRITE);
    SPI.transfer(byte(address>>16));
    temp = word(address&0xFFFF);
    SPI.transfer(highByte(temp));
    SPI.transfer(lowByte(temp));
    SPI.transfer(value);
    digitalWrite(RWM_EEPROM_CS, HIGH);
  }
  else {
    return 0xFF;
  }
}

void RWM::EEPROMwriteBytes(unsigned long address, byte n, byte *buffer) {
  word temp;
  byte i;

  if ((address+(unsigned long)n)<=0x20000) {
    while (EEPROMreadStatus()&0x01) {}
    EEPROMwriteEnable();
    digitalWrite(RWM_EEPROM_CS, LOW);
    SPI.transfer(RWM_EEPROM_WRITE);
    SPI.transfer(byte(address>>16));
    temp = word(address&0xFFFF);
    SPI.transfer(highByte(temp));
    SPI.transfer(lowByte(temp));
    for (i = 0; i<n; i++)
      SPI.transfer(buffer[i]);
    digitalWrite(RWM_EEPROM_CS, HIGH);
  }
}

byte RWM::EEPROMreadByte(unsigned long address) {
  word temp;
  byte value;
  
  if (address<=0x1FFFF) {
    digitalWrite(RWM_EEPROM_CS, LOW);
    SPI.transfer(RWM_EEPROM_READ);
    SPI.transfer(byte(address>>16));
    temp = word(address&0xFFFF);
    SPI.transfer(highByte(temp));
    SPI.transfer(lowByte(temp));
    value = SPI.transfer(0);
    digitalWrite(RWM_EEPROM_CS, HIGH);
    return value;
  } else
    return 0xFF;
}

void RWM::EEPROMwriteInt(unsigned long address, int value) {
  word temp;

  address = address<<1;
  if (address<0x1FFFC) {
    while (EEPROMreadStatus()&0x01) {}
    EEPROMwriteEnable();
    digitalWrite(RWM_EEPROM_CS, LOW);
    SPI.transfer(RWM_EEPROM_WRITE);
    SPI.transfer(byte(address>>16));
    temp = word(address&0xFFFF);
    SPI.transfer(highByte(temp));
    SPI.transfer(lowByte(temp));
    SPI.transfer(lowByte(value));
    SPI.transfer(highByte(value));
    digitalWrite(RWM_EEPROM_CS, HIGH);
  }
}

void RWM::EEPROMwriteInts(unsigned long address, byte n, int *buffer) {
  word temp;
  byte i;

  address = address<<1;
  if ((address+(((unsigned long)n)<<1))<=0x1FFFC) {
    while (EEPROMreadStatus()&0x01) {}
    EEPROMwriteEnable();
    digitalWrite(RWM_EEPROM_CS, LOW);
    SPI.transfer(RWM_EEPROM_WRITE);
    SPI.transfer(byte(address>>16));
    temp = word(address&0xFFFF);
    SPI.transfer(highByte(temp));
    SPI.transfer(lowByte(temp));
    for (i = 0; i<n; i++) {
      SPI.transfer(lowByte(buffer[i]));
      SPI.transfer(highByte(buffer[i]));
    }
    digitalWrite(RWM_EEPROM_CS, HIGH);
  }
}

int RWM::EEPROMreadInt(unsigned long address) {
  word temp;
  byte value;
  
  address = address<<1;
  if (address<0x1FFFC) {
    digitalWrite(RWM_EEPROM_CS, LOW);
    SPI.transfer(RWM_EEPROM_READ);
    SPI.transfer(byte(address>>16));
    temp = word(address&0xFFFF);
    SPI.transfer(highByte(temp));
    SPI.transfer(lowByte(temp));
    value = SPI.transfer(0);
    temp = word(SPI.transfer(0), value);
    digitalWrite(RWM_EEPROM_CS, HIGH);
    return int(temp);
  } else
    return 0;
}