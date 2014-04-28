#include <SPI.h>
#include <RWM2.h>

RWM rwm = RWM2();

#define DATA_BUFFER_SIZE  16

int ao0 = 0;
int ao1 = 0;
unsigned long interval = 1;
unsigned long samples = 1000;
unsigned long address = 0;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  int towrite = 1;
  while (address<65534) {
    t = millis();
    if (t>=t0) {
      rwm.EEPROMwriteInt(address, towrite);
      int written = rwm.EEPROMreadInt(address);
      Serial.write("the int is"+String(written))
      address += DATA_BUFFER_SIZE;
      i = 0;        
      t0 = t+interval;
    }
  }
}

void loop() {
  // put your main code here, to run repeatedly: 
  
}
