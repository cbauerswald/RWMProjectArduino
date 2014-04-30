#include <SPI.h>
#include <RWM.h>

RWM rwm = RWM();

#define DATA_BUFFER_SIZE  16

int led_red = 4;
int led_green = 6;

int t=0;
int t0 =0;
int ao0 = 0;
int ao1 = 0;
unsigned long interval = 1;
unsigned long samples = 1000;
unsigned long address = 64;


void setup() {
  // put your setup code here, to run once:
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH);
  Serial.println("We are starting to do things.");
  // eeprom cs is 2
  // adxl cs is 5
  pinMode(2, OUTPUT); 
  digitalWrite(2, HIGH);
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  Serial.println("the cs pins have been set");
  
  //digitalWrite(2, LOW);
  //SPI.transfer(RWM_CE);
  //digitalWrite(2, HIGH);
  
  Serial.begin(115200);
};

void loop() {
  Serial.println("in the loop");
  // put your main code here, to run repeatedly:
  /*digitalWrite(2,LOW);
  int written = rwm.EEPROMreadInt(address);
  digitalWrite(2,HIGH);
  delay(10);*/
  /*int values[6];
  digitalWrite(5, LOW);
  rwm.readAccelerometer();
  digitalWrite(5, HIGH);*/
  digitalWrite(2, LOW);
  Serial.println("about to try to read");
  int val = rwm.EEPROMreadByte(address);
  Serial.println("just read");
  digitalWrite(2,HIGH);
  
  
  /*if (val>0) {
    digitalWrite(4, HIGH);
  }
  else {
   digitalWrite(4, LOW);
  }*/
  //Serial.println(val);
  //digitalWrite(led_red, LOW);
  delay(100);               // wait for a second
  //digitalWrite(led_red, HIGH);   // turn the LED on (HIGH is the voltage level)
  //digitalWrite(led_red, LOW);    // turn the LED off by making the voltage LOW
  //digitalWrite(led_green, HIGH);
  //delay(1000);               // wait for a second
}
