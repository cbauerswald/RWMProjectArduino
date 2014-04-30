#include <SPI.h>

#define DATAOUT 11//MOSI
#define DATAIN  12//MISO 
#define SPICLOCK  13//sck

#define EEPROM_CS     2         // Chip select pin for EEPROM
#define  ADXL_CS      5         // Chip select pin for accelerometer, ADXL345

#define  ADXL_DATAFORMAT    0x31        // set the data format thing
#define  ADXL_DATAX0        0x32       // X-Axis Data 0
#define  ADXL_DATAX1        0x33       // X-Axis Data 1
#define  ADXL_DATAY0        0x34       // Y-Axis Data 0
#define  ADXL_DATAY1        0x35       // Y-Axis Data 1
#define  ADXL_DATAZ0        0x36       // Z-Axis Data 0
#define  ADXL_DATAZ1        0x37       // Z-Axis Data 1
#define  EEPROM_READ   0b00000011       // Read data from memory array beginning at selected address
#define  EEPROM_WRITE  0b00000010       // Write data to memory array beginning at selected address
#define  EEPROM_WREN   0b00000110       // Set the write enable latch (enable write operations)
#define  RDSR           0b00000101      // Read STATUS register
#define  WRSR           0b00000001      // Write STATUS register
#define  CE             0b11000111      // Chip Erase - erase all sectors in memory array
#define  RDID           0b10101011      // Release from Deep power-down and read electronic signature
#define  DPD            0b10111001      // Deep Power-Down mode


byte eeprom_output_data;
byte eeprom_input_data=0;
byte clr;
int address=0;
//data buffer
char buffer [128]; //this is a 128 byte array we will be using to store the data for the EEPROM write

void fill_buffer()
{
  
  for (int I=0;I<128;I++)
  {
    buffer[I]=I;
  }
}

/*char spi_transfer(volatile char data)
{
  SPDR = data;                    // Start the transmission
  while (!(SPSR & (1<<SPIF)))     // Wait for the end of the transmission
  {
  };
  return SPDR;                    // return the received byte
}

byte read_eeprom(int EEPROM_address)
{
  //READ EEPROM
  int data;
  digitalWrite(EEPROM_CS,LOW);
  spi_transfer(EEPROM_READ); //transmit read opcode
  spi_transfer((char)(EEPROM_address>>8));   //send MSByte address first
  spi_transfer((char)(EEPROM_address));      //send LSByte address
  data = spi_transfer(0xFF); //get data byte
  digitalWrite(EEPROM_CS, HIGH); //release chip, signal end transfer
  return data;
} 
*/

void setup() {
  Serial.begin(9600);
  Serial.println("Program Just Started.");
  delay(4000);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  Serial.println("4 second delay completed, to allow time for opening serial monitor.");
  delay(500);
 
  
  pinMode(DATAOUT, OUTPUT);
  pinMode(DATAIN, INPUT);
  pinMode(EEPROM_CS, OUTPUT);
  pinMode(SPICLOCK, OUTPUT);
  digitalWrite(EEPROM_CS, HIGH);
  pinMode(ADXL_CS, OUTPUT);
  digitalWrite(ADXL_CS, HIGH);
  
  SPI.begin();
  //SPI.setDataMode(SPI_MODE3); //see arduino spi page for more info, the EEPROM is in this data mode and the ADXL is set to be in this data mode in writeregister
  SPI.setClockDivider(SPI_CLOCK_DIV8); //this was in Brad's code, so I did it the same way...
  Serial.println("SPI has been initialized");
  delay(500);
  
  SPCR = 01010000;
  //interrupt disabled,spi enabled,msb 1st,master,clk low when idle,
  //sample on leading edge of clk,system clock/4 rate (fastest)
  //SPCR = (1<<SPE)|(1<<MSTR);
  
  //clear out any spurious data from past runs
  clr=SPSR;
  clr=SPDR;
  delay(10); 

  
  
  //fill buffer with data
  fill_buffer();
  Serial.println("Buffer was just filled.");
  delay(1000);
  
  //fill eeprom w/ buffer

  
  Serial.println("EEPROM has been enabled.");
  delay(1000);
  
  
  /*delay(10);
  digitalWrite(EEPROM_CS,LOW);
  spi_transfer(EEPROM_WRITE); //write instruction
  address=0;
  spi_transfer((char)(address>>8));   //send MSByte address first
  spi_transfer((char)(address));      //send LSByte address
  //write 128 bytes
  for (int I=0;I<128;I++)
  {
    spi_transfer(buffer[I]); //write data byte
  }
  digitalWrite(EEPROM_CS,HIGH); //release chip
  //wait for eeprom to finish writing
  delay(3000);
  
  Serial.println('hi');
  delay(1000);*/
  
}
int counter_for_testing = 0;
void loop() {
  
  digitalWrite(EEPROM_CS,LOW);
  delay(10);
  Serial.println("Write pin is low.");
  delay(500);
  SPI.transfer(EEPROM_WREN);
  Serial.println("Transfered enable byte");
  delay(500);
  //spi_transfer(EEPROM_WREN); //write enable
  digitalWrite(EEPROM_CS,HIGH);
  
  counter_for_testing ++;
  Serial.print("In the loop, know what i mean ;). Counter value: ");
  Serial.println(counter_for_testing);
  /*eeprom_output_data = read_eeprom(address);
  Serial.println(eeprom_output_data,DEC);
  address++;*/
  delay(500); //pause for readability
}
