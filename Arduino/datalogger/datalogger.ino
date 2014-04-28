#include <SPI.h>
#include <RWM.h>

RWM rwm = RWM();

#define DATA_BUFFER_SIZE  16

int ao0 = 0;
int ao1 = 0;
unsigned long interval = 1;
unsigned long samples = 1000;
unsigned long address = 0;

void logdata() {
  byte i;
  int databuffer[DATA_BUFFER_SIZE];
  unsigned long t0, t;

  t0 = millis()+interval;
  i = 0;
  address = 0;
  while (address<65534) {
    t = millis();
    if (t>=t0) {
      databuffer[i++] = rwm.ADCreadChannel(0);
      if (i==DATA_BUFFER_SIZE) {
        rwm.EEPROMwriteInts(address, DATA_BUFFER_SIZE, databuffer);
        address += DATA_BUFFER_SIZE;
        i = 0;        
      }
      t0 = t+interval;
    }
  }
}

void dumpdata() {
  unsigned long t;

  t = 0;
  interval = (long(rwm.EEPROMreadByte(0x1FFFF))<<16)|long(word(rwm.EEPROMreadByte(0x1FFFE), rwm.EEPROMreadByte(0x1FFFD)));
  for (address = 0; address<65534; address++) {
    Serial.print(t);
    Serial.print(",");
    Serial.println(rwm.EEPROMreadInt(address));
    t += interval;
  }
}

void command() {
  char buffer[64];
  int i, ch, val;
  long j, num;

  for (i = 0; i<63; i++) {
    for (ch = Serial.read(); ch==-1; ch = Serial.read()) {}
    if (ch==';')
      break;
    buffer[i] = (char)ch;
  }
  buffer[i] = '\0';
  if ((!strncmp(buffer, "ao0?", 4)) || (!strncmp(buffer, "AO0?", 4))) {
    Serial.print("AO0 = ");
    Serial.println(ao0, DEC);
  } else if ((!strncmp(buffer, "ao0", 3)) || (!strncmp(buffer, "AO0", 3))) {
    ao0 = atoi(buffer+3);
    rwm.DACwriteChannel(0, ao0);
  } else if ((!strncmp(buffer, "ao1?", 4)) || (!strncmp(buffer, "AO1?", 4))) {
    Serial.print("AO1 = ");
    Serial.println(ao1, DEC);
  } else if ((!strncmp(buffer, "ao1", 3)) || (!strncmp(buffer, "AO1", 3))) {
    ao1 = atoi(buffer+3);
    rwm.DACwriteChannel(1, ao1);
  } else if ((!strncmp(buffer, "ai0?", 4)) || (!strncmp(buffer, "AI0?", 4))) {
    Serial.print("AI0 = ");
    Serial.println(rwm.ADCreadChannel(0), DEC);
  } else if ((!strncmp(buffer, "ai1?", 4)) || (!strncmp(buffer, "AI1?", 4))) {
    Serial.print("AI1 = ");
    Serial.println(rwm.ADCreadChannel(1), DEC);
  } else if ((!strncmp(buffer, "ai2?", 4)) || (!strncmp(buffer, "AI2?", 4))) {
    Serial.print("AI2 = ");
    Serial.println(rwm.ADCreadChannel(2), DEC);
  } else if ((!strncmp(buffer, "ai3?", 4)) || (!strncmp(buffer, "AI3?", 4))) {
    Serial.print("AI3 = ");
    Serial.println(rwm.ADCreadChannel(3), DEC);
  } else if ((!strncmp(buffer, "aid0?", 5)) || (!strncmp(buffer, "AID0?", 5))) {
    Serial.print("AID0 = ");
    Serial.println(rwm.ADCreadChannelDiff(0), DEC);
  } else if ((!strncmp(buffer, "aid1?", 5)) || (!strncmp(buffer, "AID1?", 5))) {
    Serial.print("AID1 = ");
    Serial.println(rwm.ADCreadChannelDiff(1), DEC);
  } else if ((!strncmp(buffer, "interval?", 9)) || (!strncmp(buffer, "INTERVAL?", 9))) {
    Serial.print("interval = ");
    Serial.println(interval, DEC);
  } else if ((!strncmp(buffer, "interval", 8)) || (!strncmp(buffer, "INTERVAL", 8))) {
    interval = (unsigned long)atol(buffer+8);
  } else if ((!strncmp(buffer, "samples?", 8)) || (!strncmp(buffer, "SAMPLES?", 8))) {
    Serial.print("samples = ");
    Serial.println(samples, DEC);
  } else if ((!strncmp(buffer, "samples", 7)) || (!strncmp(buffer, "SAMPLES", 7))) {
    samples = (unsigned long)atol(buffer+7);
  } else if ((!strncmp(buffer, "clear", 5)) || (!strncmp(buffer, "CLEAR", 5))) {
    rwm.EEPROMwriteEnable();
    rwm.EEPROMchipErase();
  } else if ((!strncmp(buffer, "address?", 8)) || (!strncmp(buffer, "ADDRESS?", 8))) {
    Serial.print("address = ");
    Serial.println(address, DEC);
  } else if ((!strncmp(buffer, "address", 7)) || (!strncmp(buffer, "ADDRESS", 7))) {
    address = (unsigned long)atol(buffer+7);
  } else if ((!strncmp(buffer, "writebyte", 9)) || (!strncmp(buffer, "WRITEBYTE", 9))) {
    val = atoi(buffer+9);
    rwm.EEPROMwriteByte(address, (byte)val);
  } else if ((!strncmp(buffer, "readbyte?", 9)) || (!strncmp(buffer, "READBYTE?", 9))) {
    Serial.print("EEPROM[");
    Serial.print(address, DEC);
    Serial.print("] = ");
    Serial.println(rwm.EEPROMreadByte(address), DEC);
  } else if ((!strncmp(buffer, "writeint", 8)) || (!strncmp(buffer, "WRITEINT", 8))) {
    val = atoi(buffer+9);
    rwm.EEPROMwriteInt(address, val);
  } else if ((!strncmp(buffer, "readint?", 8)) || (!strncmp(buffer, "READINT?", 8))) {
    Serial.print("EEPROM[");
    Serial.print(address, DEC);
    Serial.print("] = ");
    Serial.println(rwm.EEPROMreadInt(address), DEC);
  } else if ((!strncmp(buffer, "readbytes", 9)) || (!strncmp(buffer, "READBYTES", 9))) {
    num = atol(buffer+9);
    for (j = 0; j<num; j++) {
      Serial.print(rwm.EEPROMreadByte(address+j), DEC);
      if (j+1==num)
        Serial.println("");
      else
        Serial.print(",");
    }
  } else if ((!strncmp(buffer, "readints", 8)) || (!strncmp(buffer, "READINTS", 8))) {
    num = atol(buffer+9);
    for (j = 0; j<num; j++) {
      Serial.print(rwm.EEPROMreadInt(address+j), DEC);
      if (j+1==num)
        Serial.println("");
      else
        Serial.print(",");
    }
  } else if ((!strncmp(buffer, "log", 3)) || (!strncmp(buffer, "LOG", 3))) {
    Serial.println("Clearing the EEPROM.");
    rwm.EEPROMwriteEnable();
    rwm.EEPROMchipErase();
    rwm.EEPROMwriteByte(0x1FFFC, 1);
    rwm.EEPROMwriteByte(0x1FFFD, lowByte(word(interval&0xFFFF)));
    rwm.EEPROMwriteByte(0x1FFFE, highByte(word(interval&0xFFFF)));
    rwm.EEPROMwriteByte(0x1FFFF, byte(interval>>16));
    Serial.println("Disconnect the USB cable and reset the Arduino to start logging data.");
  } else if ((!strncmp(buffer, "dump", 4)) || (!strncmp(buffer, "DUMP", 4))) {
    dumpdata();
  }
}

void setup() {
  Serial.begin(115200);
  if ((rwm.EEPROMreadByte(0x1FFFC)==1) && (rwm.EEPROMreadByte(0)==255)) {
    interval = (long(rwm.EEPROMreadByte(0x1FFFF))<<16)|long(word(rwm.EEPROMreadByte(0x1FFFE), rwm.EEPROMreadByte(0x1FFFD)));
    logdata();
  }
}

void loop() {
  if (Serial.available()>0) {
    command();
  }
}

