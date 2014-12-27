/*
*   ____               _____         _
*  / ___|___  ___ _   |_   _|__  ___| |__
* | |   / _ \/ __| | | || |/ _ \/ __| '_ \
* | |__| (_) \__ \ |_| || |  __/ (__| | | |
*  \____\___/|___/\__, ||_|\___|\___|_| |_|
*                 |___/
*
* CosyTech Skunkworks - Bluetooth HC06 Testprogram
* Requires SPP Support at device
* By Joakim Skjefstad (joakim@cosytech.net)
* 
* Pins used:
* 
*/

#include <SoftwareSerial.h>

const int led = 12;           // PulseLed
int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by
  
void setup()
{
    pinMode(led, OUTPUT);   // sets the pin as output
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for Leonardo only
  }
 
  Serial.println("Ready!");
  // set the data rate for the SoftwareSerial port
 
  // for HC-05 use 38400 when poerwing with KEY/STATE set to HIGH on power on
  Serial1.begin(9600);
}
 
void loop() // run over and over
{
  if (Serial1.available()){
    Serial.write(Serial1.read());
    pulse_led();
  }
  if (Serial.available()){
    Serial1.write(Serial.read());
  }
}

void pulse_led(){
  Serial.println("Pulsing LED");
  for (int i = 0; i < 255; i++){
    analogWrite(led, i);

  }
  for (int i = 255; i > 0; i--){
    analogWrite(led, i);
    delay(1);
  }
}