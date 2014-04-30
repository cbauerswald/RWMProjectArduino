#ifndef TR_h
#define TR_h

#include "Arduino.h"
#include "SPI.h"

#define TR_EEPROM_CS     2         // Chip select pin for EEPROM
#define TR_ADXL1_CS      5         // Chip select pin for accelerometer, ADXL345

#define TR_ADXL_DATAFORMAT    0x31        // set the data format thing
#define TR_ADXL_DATAX0        0x32       // X-Axis Data 0
#define TR_ADXL_DATAX1        0x33       // X-Axis Data 1
#define TR_ADXL_DATAY0        0x34       // Y-Axis Data 0
#define TR_ADXL_DATAY1        0x35       // Y-Axis Data 1
#define TR_ADXL_DATAZ0        0x36       // Z-Axis Data 0
#define TR_ADXL_DATAZ1        0x37       // Z-Axis Data 1
#define TR_EEPROM_READ   0b00000011       // Read data from memory array beginning at selected address
#define TR_EEPROM_WRITE  0b00000010       // Write data to memory array beginning at selected address
#define TR_EEPROM_WREN   0b00000110       // Set the write enable latch (enable write operations)
#define TR_RDSR           0b00000101      // Read STATUS register
#define TR_WRSR           0b00000001      // Write STATUS register
#define TR_CE             0b11000111      // Chip Erase - erase all sectors in memory array
#define TR_RDID           0b10101011      // Release from Deep power-down and read electronic signature
#define TR_DPD            0b10111001      // Deep Power-Down mode

class TR {
  public:
    TR();
    byte EEPROMreadStatus(void);
    void EEPROMwriteEnable(void);
    void EEPROMchipErase(void);
    void EEPROMwriteByte(unsigned long address, byte value);
    void EEPROMwriteBytes(unsigned long address, byte num, byte *buffer);
    byte EEPROMreadByte(unsigned long address);
    void EEPROMwriteInt(unsigned long address, int value);
    void EEPROMwriteInts(unsigned long address, byte num, int *buffer);
    int EEPROMreadInt(unsigned long address);
    void readAccelerometer(void);
    void TRwriteRegister(char registerAddress, char value);
  private:
};

#endif
