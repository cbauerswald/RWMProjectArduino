#ifndef RWM_h
#define RWM_h

#include "Arduino.h"
#include "SPI.h"

#define RWM2_EEPROM_CS     1         // Chip select pin for EEPROM
#define RWM2_ADXL_CS       7         // Chip select pin for accelerometer, ADXL345

#define RWM2_ADXL_DATAFORMAT    0x31        // set the data format thing
#define RWM2_ADXL_DATAX0        0x32       // X-Axis Data 0
#define RWM2_ADXL_DATAX1        0x33       // X-Axis Data 1
#define RWM2_ADXL_DATAY0        0x34       // Y-Axis Data 0
#define RWM2_ADXL_DATAY1        0x35       // Y-Axis Data 1
#define RWM2_ADXL_DATAZ0        0x36       // Z-Axis Data 0
#define RWM2_ADXL_DATAZ1        0x37       // Z-Axis Data 1
#define RWM2_EEPROM_READ   0b00000011       // Read data from memory array beginning at selected address
#define RWM2_EEPROM_WRITE  0b00000010       // Write data to memory array beginning at selected address
#define RWM2_EEPROM_WREN   0b00000110       // Set the write enable latch (enable write operations)
#define RWM2_RDSR           0b00000101      // Read STATUS register
#define RWM2_WRSR           0b00000001      // Write STATUS register
#define RWM2_CE             0b11000111      // Chip Erase - erase all sectors in memory array
#define RWM2_RDID           0b10101011      // Release from Deep power-down and read electronic signature
#define RWM2_DPD            0b10111001      // Deep Power-Down mode

class RWM {
  public:
    RWM2();
    void DACwriteChannel(byte ch, int value);
    void DACwriteChannels(int value0, int value1);
    int ADCreadChannel(byte ch);
    int ADCreadChannelDiff(byte ch);
    byte EEPROMreadStatus(void);
    void EEPROMwriteEnable(void);
    void EEPROMchipErase(void);
    void EEPROMwriteByte(unsigned long address, byte value);
    void EEPROMwriteBytes(unsigned long address, byte num, byte *buffer);
    byte EEPROMreadByte(unsigned long address);
    void EEPROMwriteInt(unsigned long address, int value);
    void EEPROMwriteInts(unsigned long address, byte num, int *buffer);
    int EEPROMreadInt(unsigned long address);
    void EEPROMwrite2Ints(unsigned long address, int *buffer);
    void EEPROMread2Ints(unsigned long address, int *buffer);
    void EEPROMwrite4Ints(unsigned long address, int *buffer);
    void EEPROMread4Ints(unsigned long address, int *buffer);
  private:
};

#endif