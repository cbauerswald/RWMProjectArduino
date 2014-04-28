/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.
 
  This example code is in the public domain.
 */
 
// Pin 13 has an LED connected on most Arduino boards.
// give it a name:
int led_red = 4;
int led_green = 6;

// the setup routine runs once when you press reset:
void setup() {                
  // initialize the digital pin as an output.
  pinMode(led_red, OUTPUT);   
  pintMode(led_green, OUTPUT);  
}

// the loop routine runs over and over again forever:
void loop() {
  digitalWrite(led_green, LOW)
  digitalWrite(led_red, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second
  digitalWrite(led_red, LOW);    // turn the LED off by making the voltage LOW
  digitalWrite(led_green, HIGH);
  delay(1000);               // wait for a second
}
