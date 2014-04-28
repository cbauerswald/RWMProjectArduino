#include "RWM.h"

RWM::RWM() {
  pinMode(RWM_DAC_CS, OUTPUT);
  digitalWrite(RWM_DAC_CS, HIGH);
  pinMode(RWM_ADC_CS, OUTPUT);
  digitalWrite(RWM_ADC_CS, HIGH);
  pinMode(RWM_EEPROM_CS, OUTPUT);
  digitalWrite(RWM_EEPROM_CS, HIGH);
  pinMode(RWM_LDAC, OUTPUT);
  digitalWrite(RWM_LDAC, HIGH);
  SPI.begin();
  SPI.setClockDivider(SPI_CLOCK_DIV8);
}

void RWM::DACwriteChannel(byte ch, int value) {
  if ((ch<2) && (value>=0) && (value<4096)) {
    digitalWrite(RWM_DAC_CS, LOW);
    SPI.transfer(((ch==0) ? 0x30:0xB0)|(highByte(value)&0x0F));
    SPI.transfer(lowByte(value));
    digitalWrite(RWM_DAC_CS, HIGH);
    digitalWrite(RWM_LDAC, LOW);
    digitalWrite(RWM_LDAC, HIGH);
  }
}

void RWM::DACwriteChannels(int value0, int value1) {
  if ((value0>=0) && (value0<4096) && (value1>=0) && (value1<4096)) {
    digitalWrite(RWM_DAC_CS, LOW);
    SPI.transfer(0x30|(highByte(value0)&0x0F));
    SPI.transfer(lowByte(value0));
    digitalWrite(RWM_DAC_CS, HIGH);
    digitalWrite(RWM_DAC_CS, LOW);
    SPI.transfer(0xB0|(highByte(value1)&0x0F));
    SPI.transfer(lowByte(value1));
    digitalWrite(RWM_DAC_CS, HIGH);
    digitalWrite(RWM_LDAC, LOW);
    digitalWrite(RWM_LDAC, HIGH);
  }
}

int RWM::ADCreadChannel(byte ch) {
  byte value;
  word temp;
  
  if (ch<4) {
    digitalWrite(RWM_ADC_CS, LOW);
    SPI.transfer(0x0C|(ch>>1));
    value = 0x1F&SPI.transfer((ch&0x01) ? 0x80:0x00);
    temp = word(value, SPI.transfer(0x00));
    digitalWrite(RWM_ADC_CS, HIGH);
    return int(temp);
  } else
    return 0x8000;
}

int RWM::ADCreadChannelDiff(byte ch) {
  byte value;
  word temp;

  if (ch<2) {
    digitalWrite(RWM_ADC_CS, LOW);
    SPI.transfer(0x08|ch);
    value = SPI.transfer(0x00);
    temp = word((value&0x1F)|((value&0x10) ? 0xE0:0x00), SPI.transfer(0x00));
    digitalWrite(RWM_ADC_CS, HIGH);
    return int(temp);
  } else
    return 0x8000;
}

byte RWM::EEPROMreadStatus(void) {
  byte value;

  digitalWrite(RWM_EEPROM_CS, LOW);
  SPI.transfer(RWM_RDSR);
  value = SPI.transfer(0);
  digitalWrite(RWM_EEPROM_CS, HIGH);
  return value;
}

void RWM::EEPROMwriteEnable(void) {
  digitalWrite(RWM_EEPROM_CS, LOW);
  SPI.transfer(RWM_WREN);
  digitalWrite(RWM_EEPROM_CS, HIGH);
}

void RWM::EEPROMchipErase(void) {
  digitalWrite(RWM_EEPROM_CS, LOW);
  SPI.transfer(RWM_CE);
  digitalWrite(RWM_EEPROM_CS, HIGH);
}

void RWM::EEPROMwriteByte(unsigned long address, byte value) {
  word temp;
  
  if (address<=0x1FFFF) {
    while (EEPROMreadStatus()&0x01) {}
    EEPROMwriteEnable();
    digitalWrite(RWM_EEPROM_CS, LOW);
    SPI.transfer(RWM_WRITE);
    SPI.transfer(byte(address>>16));
    temp = word(address&0xFFFF);
    SPI.transfer(highByte(temp));
    SPI.transfer(lowByte(temp));
    SPI.transfer(value);
    digitalWrite(RWM_EEPROM_CS, HIGH);
  }
}

void RWM::EEPROMwriteBytes(unsigned long address, byte n, byte *buffer) {
  word temp;
  byte i;

  if ((address+(unsigned long)n)<=0x20000) {
    while (EEPROMreadStatus()&0x01) {}
    EEPROMwriteEnable();
    digitalWrite(RWM_EEPROM_CS, LOW);
    SPI.transfer(RWM_WRITE);
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
    SPI.transfer(RWM_READ);
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
    SPI.transfer(RWM_WRITE);
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
    SPI.transfer(RWM_WRITE);
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
    SPI.transfer(RWM_READ);
    SPI.transfer(byte(address>>16));
    temp = word(address&0xFFFF);
    SPI.transfer(highByte(temp));
    SPI.transfer(lowByte(temp));
    value = SPI.transfer(0);
    temp = word(SPI.transfer(0), value);
    digitalWrite(RWM_EEPROM_CS, HIGH);
    return int(temp);
  } else
    return 0xFFFF;
}

void RWM::EEPROMwrite2Ints(unsigned long address, int *buffer) {
  word temp;
  byte i;
  
  address = address<<2;
  if (address<0x1FFFC) {
    while (EEPROMreadStatus()&0x01) {}
    EEPROMwriteEnable();
    digitalWrite(RWM_EEPROM_CS, LOW);
    SPI.transfer(RWM_WRITE);
    SPI.transfer(byte(address>>16));
    temp = word(address&0xFFFF);
    SPI.transfer(highByte(temp));
    SPI.transfer(lowByte(temp));
    for (i = 0; i<2; i++) {
      SPI.transfer(lowByte(buffer[i]));
      SPI.transfer(highByte(buffer[i]));
    }
    digitalWrite(RWM_EEPROM_CS, HIGH);
  }
}

void RWM::EEPROMread2Ints(unsigned long address, int *buffer) {
  word temp;
  byte i, value;
  
  address = address<<2;
  if (address<0x1FFFC) {
    digitalWrite(RWM_EEPROM_CS, LOW);
    SPI.transfer(RWM_READ);
    SPI.transfer(byte(address>>16));
    temp = word(address&0xFFFF);
    SPI.transfer(highByte(temp));
    SPI.transfer(lowByte(temp));
    for (i = 0; i<2; i++) {
      value = SPI.transfer(0);
      temp = word(SPI.transfer(0), value);
      buffer[i] = int(temp);
    }
    digitalWrite(RWM_EEPROM_CS, HIGH);
  } else {
    for (i = 0; i<2; i++)
      buffer[i] = 0xFFFF;
  }
}

void RWM::EEPROMwrite4Ints(unsigned long address, int *buffer) {
  word temp;
  byte i;
  
  address = address<<3;
  if (address<0x1FFF8) {
    while (EEPROMreadStatus()&0x01) {}
    EEPROMwriteEnable();
    digitalWrite(RWM_EEPROM_CS, LOW);
    SPI.transfer(RWM_WRITE);
    SPI.transfer(byte(address>>16));
    temp = word(address&0xFFFF);
    SPI.transfer(highByte(temp));
    SPI.transfer(lowByte(temp));
    for (i = 0; i<4; i++) {
      SPI.transfer(lowByte(buffer[i]));
      SPI.transfer(highByte(buffer[i]));
    }
    digitalWrite(RWM_EEPROM_CS, HIGH);
  }
}

void RWM::EEPROMread4Ints(unsigned long address, int *buffer) {
  word temp;
  byte i, value;
  
  address = address<<3;
  if (address<0x1FFF8) {
    digitalWrite(RWM_EEPROM_CS, LOW);
    SPI.transfer(RWM_READ);
    SPI.transfer(byte(address>>16));
    temp = word(address&0xFFFF);
    SPI.transfer(highByte(temp));
    SPI.transfer(lowByte(temp));
    for (i = 0; i<4; i++) {
      value = SPI.transfer(0);
      temp = word(SPI.transfer(0), value);
      buffer[i] = int(temp);
    }
    digitalWrite(RWM_EEPROM_CS, HIGH);
  } else {
    for (i = 0; i<4; i++)
      buffer[i] = 0xFFFF;
  }
}
