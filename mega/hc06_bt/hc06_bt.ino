#include <SoftwareSerial.h>
  
void setup()
{
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
  if (Serial1.available())
    Serial.write(Serial1.read());
  if (Serial.available())
    Serial1.write(Serial.read());
}