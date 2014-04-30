// 25LC1024 1Mbit SPI interface
//
// Device pin connections:
// 1 = CS (Chip Select) (from Arduino)  
// 2 = SO (Master In Slave Out) (from Arduino)
// 3 = +5V (WP) (from Arduino)
// 4 = 0V (Ground) (from Arduino)
// 5 = SI (Master Out Slave In) (from Arduino)
// 6 = SCK (from Arduino)
// 7 = +5V HOLD (from Arduino)
// 8 = +5V Vcc (from Arduino)
//
// This file uses the SPI library and produces higher oscillation rates than the manual methods
// for the Uno: SS/CS = pin 10, MOSI/SI = pin 11, SCK/clock = pin 13, MISO/SO = pin = 12
// 

// For the EEPROM, basic commands
#define EEPROM_WRITE 2// Write data to memory array beginning at selected address
#define EEPROM_READ  3// Read data from memory array beginning at selected address
#define EEPROM_WRITE_ENABLE  6// Set the write enable latch (enable write operations)
#define EEPROM_WRITE_DISABLE   0b00000100     // Reset the write enable latch (disable write operations)
#define EEPROM_READ_STATUS_REGISTER   0b00000101     // Read STATUS register
#define EEPROM_WRITE_STATUS_REGISTER   0b00000001     // Write STATUS register
#define EEPROM_PAGE_ERASE     0b01000010     // Page Erase - erase one page in memory array
#define EEPROM_SECTOR_ERASE     0b11011000     // Sector Erase - erase one sector in memory array
#define EEPROM_CHIP_ERASE     0b11000111     // Chip Erase - erase all sectors in memory array
#define EEPROM_RDID   0b10101011     // Release from Deep power-down and read electronic signature
#define EEPROM_DEEP_POWER_DOWN    0b10111001     // Deep Power-Down mode

// pinouts
#define EEPROM_CS 2
#define AXLM_CS 5

unsigned long EEPROM_address = 0; // pointer keeps track of where the data is.
int currentIndex = 0;


//INITIALIZATION FOR EEPROM
#define WRITE 2
#define READ  3
#define WREN  6

#define EEPROMSELECT 2
#define AXLMSELECT 5
#define BUTTONPIN 3

#include <SPI.h>

unsigned int data;
unsigned int address = 0;
unsigned int outval = 23;
byte invalue;

//cecelia's stuff
boolean idlemode = true;
boolean logdelay = false;
boolean logdata = false;
boolean logdatalow = false;
boolean dumpdata = false;
boolean highfreq = true;  
unsigned long t0 = 0;
unsigned long t = 0;
unsigned long delaytime = 3*1000; //3 seconds
unsigned long tElapsed;
unsigned long tElapsedLS;
int led_red =4;
int led_green=6;
int buttonState = 0; // variable for reading the pushbutton status
unsigned int freqdelay = 2500;
byte dataBuffer[64];
byte tElapsedByte;
boolean debug = false;
unsigned char accVals[10];


//INITIALIZATION STUFF FOR ADXL

char POWER_CTL = 0x2D;	//Power Control Register
char DATA_FORMAT = 0x31;
char DATAX0 = 0x32;	//X-Axis Data 0
char DATAX1 = 0x33;	//X-Axis Data 1
char DATAY0 = 0x34;	//Y-Axis Data 0
char DATAY1 = 0x35;	//Y-Axis Data 1
char DATAZ0 = 0x36;	//Z-Axis Data 0
char DATAZ1 = 0x37;	//Z-Axis Data 1

//This buffer will hold values read from the ADXL345 registers.
unsigned char values[10];
//These variables will be used to hold the x,y and z axis accelerometer values.
int x,y,z;


void setup()
{
  //initialize LEDs
  pinMode(led_red, OUTPUT);
  digitalWrite(led_red, LOW);
  pinMode(led_green, OUTPUT);
  digitalWrite(led_green, LOW);
  //initialize button
  pinMode(BUTTONPIN, INPUT); 
  
  Serial.begin(57600);
  delay(4000);
  if (debug)  Serial.println("--------------");
  if (debug)  Serial.println(EEPROMSELECT); // chip select, EEPROM
  if (debug)  Serial.println(AXLMSELECT); // chip select, AXLM
  if (debug)  Serial.println(MOSI); // master out, slave in
  if (debug)  Serial.println(MISO); // master in, slave out
  if (debug)  Serial.println(SCK); // clock
  if (debug)  Serial.println("--------------");
  
  // set up to match device datasheet
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE3);
  SPI.setClockDivider(SPI_CLOCK_DIV2); // max clock is 20MHz, so can set high speed
  SPI.begin(); // sets up pin modes etc.
  
  // ADDDED BY KYLE:
  // since we don't use pin 10 as SS
  pinMode(EEPROMSELECT, OUTPUT);
  digitalWrite(EEPROMSELECT, HIGH);
  pinMode(AXLMSELECT, OUTPUT);
  digitalWrite(AXLMSELECT, HIGH);
  delay(500);
  
  //Put the ADXL345 into +/- 4G range by writing the value 0x01 to the DATA_FORMAT register.
  writeRegister(DATA_FORMAT, B00000011);
  //Put the ADXL345 into Measurement Mode by writing 0x08 to the POWER_CTL register.
  writeRegister(POWER_CTL, 0x08);  //Measurement mode  
  
  // Enable writing
  //EEPROMwriteEnablee();
  //EEPROMchipErasee();
}

void loop()
{
  //stay in idle mode until either button is pressed or computer interrupts
  while(idlemode) {
    delay(300);
    Serial.println("entering idle mode");
    buttonState = digitalRead(BUTTONPIN);
    if ( buttonState==HIGH ) {
      Serial.println("button is pressed");
      idlemode=false;
      logdelay = true;
    }
    // check if the computer has requested a read
    else if(Serial.read()== 50) {
      Serial.println("Data Dump Requested.");
      idlemode=false;
      dumpdata = true;
      //print all data and return to loop
    }
  }
  if (dumpdata) {
   //read from eeprom
   /*for each piece of data from eeprom, print it out.*/
   Serial.println("dump data entered");
   byte temp_data = 0;
   for (unsigned long temp_counter = 0; temp_counter<120000; temp_counter++) {
     Serial.print("The ");
     Serial.print(temp_counter);
     Serial.print(" byte: ");
     temp_data = EEPROMreadBytee(temp_counter);
     Serial.println(temp_data);
   }
   delay(5000);
   dumpdata = false;
   idlemode = true;
  }
  // if button has been pressed, set the time and turn on the red LED to indicate 5 minutes until data collection
  if (logdelay) {
    t0 = millis();
    digitalWrite(led_red, HIGH);
  }
  //wait until 5 minutes have passed and then exit loop
  //as exiting loop, change LEDs, change logdelay, and set logdata true
  while(logdelay) {
    t=millis();
    if (t-t0>=delaytime) {
      if (debug)  Serial.println("done with log delay");
      logdelay=false;
      logdata=true;
      EEPROMwriteEnablee();
      EEPROMchipErasee();
    }
  }
  if (logdata) {
    digitalWrite(led_red, LOW);
    digitalWrite(led_green, HIGH);
    //t0=millis();
    t0 = millis();
    if (debug) Serial.print("t0 just assigned for the data logging loop. t0: ");
    if (debug) Serial.print(t0);
    if (debug)  Serial.println(" ms");
  }
  while(logdata) {
    //t=millis();
    t=millis();
    if (debug) Serial.print("t just assigned for current loop. t: ");
    if (debug) Serial.print(t);
    if (debug)  Serial.println(" ms");
    currentIndex = logData(t, t0, dataBuffer, currentIndex);
    if (currentIndex>=63) {
      if (debug)  Serial.println("Writing Data to EEPROM.");
      dataBuffer[currentIndex] = B00000000;
      EEPROMdumpData(dataBuffer);
      currentIndex=0;
    }
    //read from accelerometer
    //write to EEPROM
    //delay(1000); //just for testing
    if (highfreq) {
      delayMicroseconds(2500); //delay for 1/400 s, f = 400
    }
    else {
      delay(20);
    }
    if (debug )Serial.print("Checking elapsed time, t-t0 = ");
    if (debug) Serial.print(t-t0);
    if (debug)  Serial.println(" ms");
    if((t-t0)>20000 && highfreq==true) { //if 20 seconds have passed
      highfreq = false;
      Serial.println("done with 400 freq");
      Serial.print("Currently, ");
      Serial.print(EEPROM_address);
      Serial.println(" Bytes have been written to memory.");
      digitalWrite(led_red, HIGH);
      //we need to make sure that this if statement is only called once in the future
    }
    if (EEPROM_address >= 120000) 
    {
      logdata= false;
      idlemode= true;
      digitalWrite(led_red, LOW);
      digitalWrite(led_green, LOW);
      Serial.println("The eeprom just filled up.");
    }
  }

  
  
  
  //when it powers on, its gonna go into an idle mode
  //in this mode it is 
  //while loop, waiting stage, that you break out of, checking for commands from computer, checking for button press, polling (is it true? is it true?)
  //2 events:
  //comptuer command, print out all data and return to loop
  //button pressed, enter delay loop, do nothing for a period of time, visual indication that you are in the waiting period, then enter datalogging loop
  //log data at 400hz for 20 seconds, involves reading from, acc, doing math to get 4 bytes (time, x, y, z), requires math bc xyz not stored in bytes, and time cannot be stored in a byte (so we do weird rollover thing)
  //when this is done, drop loggin rate, log at 50 Hz until EEPROM is full 
  //when that is done, go back to initial loop, go back to different loop where you cannot press button (won't acc wipe data) OR ideally, go into deep sleep mode after a couple minutes
  //computer command -> getting the data from the computer, over serial, some set of bytes that would be printed over serial, if you get them, start printing all of your memoriy over serial, something sent in from other code
  //read data from accelerometer
  //write data onto eeprom
}

int logData(unsigned long currentTime, unsigned long startTime, byte *buffer, int index) {
  //Serial.println("logging data");
  readRegister(DATAX0, 6, accVals);//function to read from accelerometer
  tElapsed=t-t0;
  tElapsedLS= tElapsed%100;
  tElapsedByte = byte(tElapsedLS);
  if (debug) Serial.print("Time byte: ");
  if (debug)  Serial.println(tElapsedLS);
  for (byte i=0; i<6; i++) {
    buffer[index] = accVals[i];
    index +=1; //increment current index in buffer array
  }
  buffer[index] = tElapsedByte;
  index+=1; // increment because you added the time on 
  return index;
  //function to write to EEPROM
}

void EEPROMdumpData(byte *buffer)
{
  EEPROMwriteBytess(EEPROM_address, 64, buffer);
  EEPROM_address += 64;
}







// *******************************************************************************************************
// EEPROM Functions!!

byte EEPROMreadStatuss(void) {
  byte value;

  digitalWrite(EEPROM_CS, LOW);
  SPI.transfer(EEPROM_READ_STATUS_REGISTER);
  value = SPI.transfer(0);
  digitalWrite(EEPROM_CS, HIGH);
  return value;
}

void EEPROMwriteEnablee(void) {
  digitalWrite(EEPROM_CS, LOW);
  SPI.transfer(EEPROM_WRITE_ENABLE);
  digitalWrite(EEPROM_CS, HIGH);
}

void EEPROMchipErasee(void) {
  digitalWrite(EEPROM_CS, LOW);
  SPI.transfer(EEPROM_CHIP_ERASE);
  digitalWrite(EEPROM_CS, HIGH);
}

void EEPROMwriteBytess(unsigned long address, byte n, byte *buffer) {
  word temp;
  byte i;
  
  if ((address+(unsigned long)n)<=0x20000) {
    while (EEPROMreadStatuss()&0x01) {} // delay until the EEPROM is ready
    
    EEPROMwriteEnablee();
  
    digitalWrite(EEPROM_CS, LOW);
    SPI.transfer(EEPROM_WRITE);
    // Send address, all 3 bits
    SPI.transfer((address >> 16) & 255);
    SPI.transfer((address >> 8) & 255);
    SPI.transfer(address & 255);
    // send data over
    for (i = 0; i<n; i++)
      {
      SPI.transfer(buffer[i]);
      }
    digitalWrite(EEPROM_CS, HIGH);
  }
}


byte EEPROMreadBytee(unsigned long address) {
  word temp;
  byte value;
  
  if (address<=0x1FFFF) {
    if (debug) Serial.print("Reading Byte!!: ");
    digitalWrite(EEPROM_CS, LOW);
    SPI.transfer(EEPROM_READ);
    SPI.transfer((address >> 16) & 255);
    SPI.transfer((address >> 8) & 255);
    SPI.transfer(address & 255);
    value = SPI.transfer(0);
    digitalWrite(EEPROM_CS, HIGH);
    return value;
  } else
    return 0xFF;
}


// *******************************************************************************************************




// *******************************************************************************************************
// Accelerometer code

void writeRegister(char registerAddress, char value){
  //Set Chip Select pin low to signal the beginning of an SPI packet.
  digitalWrite(AXLMSELECT, LOW);
  //Transfer the register address over SPI.
  SPI.transfer(registerAddress);
  //Transfer the desired register value over SPI.
  SPI.transfer(value);
  //Set the Chip Select pin high to signal the end of an SPI packet.
  digitalWrite(AXLMSELECT, HIGH);
}

//This function will read a certain number of registers starting from a specified address and store their values in a buffer.
//Parameters:
//  char registerAddress - The register addresse to start the read sequence from.
//  int numBytes - The number of registers that should be read.
//  char * values - A pointer to a buffer where the results of the operation should be stored.


void readRegister(char registerAddress, int numBytes, unsigned char * values){
  //Since we're performing a read operation, the most significant bit of the register address should be set.
  char address = 0x80 | registerAddress;
  //If we're doing a multi-byte read, bit 6 needs to be set as well.
  if(numBytes > 1)address = address | 0x40;
  
  //Set the Chip select pin low to start an SPI packet.
  digitalWrite(AXLMSELECT, LOW);
  //Transfer the starting register address that needs to be read.
  SPI.transfer(address);
  //Continue to read registers until we've read the number specified, storing the results to the input buffer.
  for(int i=0; i<numBytes; i++){
    values[i] = SPI.transfer(0x00);
  }
  //Set the Chips Select pin high to end the SPI packet.
  digitalWrite(AXLMSELECT, HIGH);
}

// *******************************************************************************************************


