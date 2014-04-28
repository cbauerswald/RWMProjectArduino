#ifndef RWM_h
#define RWM_h

#include "Arduino.h"
#include "SPI.h"

#define RWM_DAC_CS     10         // Chip select pin for DAC
#define RWM_ADC_CS      9         // Chip select pin for ADC
#define RWM_EEPROM_CS   8         // Chip select pin for EEPROM
#define RWM_LDAC        7         // Load DAC control signal for DAC

#define RWM_READ   0b00000011     // Read data from memory array beginning at selected address
#define RWM_WRITE  0b00000010     // Write data to memory array beginning at selected address
#define RWM_WREN   0b00000110     // Set the write enable latch (enable write operations)
#define RWM_WRDI   0b00000100     // Reset the write enable latch (disable write operations)
#define RWM_RDSR   0b00000101     // Read STATUS register
#define RWM_WRSR   0b00000001     // Write STATUS register
#define RWM_PE     0b01000010     // Page Erase - erase one page in memory array
#define RWM_SE     0b11011000     // Sector Erase - erase one sector in memory array
#define RWM_CE     0b11000111     // Chip Erase - erase all sectors in memory array
#define RWM_RDID   0b10101011     // Release from Deep power-down and read electronic signature
#define RWM_DPD    0b10111001     // Deep Power-Down mode

class RWM {
  public:
    RWM();
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
