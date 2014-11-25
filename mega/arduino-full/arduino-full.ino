/*
*   ____               _____         _     
*  / ___|___  ___ _   |_   _|__  ___| |__  
* | |   / _ \/ __| | | || |/ _ \/ __| '_ \ 
* | |__| (_) \__ \ |_| || |  __/ (__| | | |
*  \____\___/|___/\__, ||_|\___|\___|_| |_|
*                 |___/                    
*
* CosyTech Skunkworks - Active NFC Full Proof of concept
* By Joakim Skjefstad (joakim@cosytech.net)
* Based on example code by Ten Wong
* Requires Arduino Mega 2560 due to SRAM
*
* Dependencies:
* https://github.com/awong1900/RF430CL330H_Shield 
* 
* Pins used by NFC Chip:
* I2C - SDA pin 20, SCL pin 21
* /RST - D4
* INT0 - D3
* VCC - 3.3v
* GND - GND
* Pins used by Ethernet Shield:
*
*
*
*/

/* Copyright 2013-2014 Ten Wong, wangtengoo7@gmail.com  
*  https://github.com/awong1900/RF430CL330H_Shield 
*  More info : http://www.elecfreaks.com
*/
//the I2C part of this code is borrowed from Adafruit_NFCShield_I2C
//link to original https://github.com/adafruit/Adafruit_NFCShield_I2C

#include <Wire.h>

#include <MemoryFree.h>

#include <SPI.h>
#include <Ethernet.h>

#include <RestClient.h>

#include <JsonParser.h>
using namespace ArduinoJson::Parser;
JsonParser<32> parser;

#include <SimpleTimer.h>

#include <RF430CL330H_Shield.h>
#include <NdefMessage.h>
#include <NdefRecord.h>

#define IRQ   (1)   //External interrupt 1 is found on pin 3
#define RESET (4)

#define BAUDRATE 115200

const int led = 12;           // PulseLed
int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by

SimpleTimer timer;

RF430CL330H_Shield nfc(IRQ, RESET);
volatile byte into_fired = 0;
uint16_t flags = 0;

RestClient client = RestClient("royrvik.org", 8102);

//EthernetClient client;
//static uint8_t mac[] = { 0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//static uint8_t ip[] = { 192, 168, 0, 100 }; // Static

char node_server[] = "royrvik.org"; // Uses DNS

String target_url = "http://google.com";

const size_t msg_len = 128;
char msg[msg_len] = "CosyTech Default Tag\0";

long fnc = 0;

void node_update() {
  print_free_memory();
  String response = "";
  int statusCode = client.get("/nodes/1/", &response);
  Serial.print(F("REST GET: "));
  Serial.println(response);

  char charBuf[128];
  response.toCharArray(charBuf, 128);
  JsonObject root = parser.parse(charBuf);
  if (!root.success())
  {
      // Parsing fail: could be an invalid JSON, or too many tokens
    Serial.println("Parsing failed, invalid JSON or too many tokens!");
  } else {
    /*
    for (JsonObjectIterator i=root.begin(); i!=root.end(); ++i)
    {
      Serial.println(i.key());
      Serial.println((char*)i.value());
    }
    */
    fnc = root["fnc"];
    strncpy(msg,root["pay"], msg_len);
    write_ndef_to_nfc((int) fnc, msg, msg_len); // This cast is BAAAAAAAAAAD
    Serial.println("New update completed.");
  }
}

void print_free_memory(){
  Serial.print(F("Free SRAM: "));
  Serial.println(freeMemory());
}

void nfc_was_read(){
  Serial.println(F("Tag was read coke_demo"));
}
void nfc_was_written(){
  Serial.println(F("Tag was written"));
}

void shutdown(){
  Serial.println(F("D"));
  for(;;)
    ;
}

void setup(void) 
{ 
  pinMode(led, OUTPUT);   // sets the pin as output
  Serial.begin(BAUDRATE);
  Serial.println(F("CosyTech Active NFC started"));
  
  client.dhcp();
  //delay(1000);

  nfc.begin();


  write_ndef_to_nfc(1, msg, msg_len);

  nfc_irq();
  timer.setInterval(1000, node_update);
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

void loop(void) 
{
  attachInterrupt(IRQ, RF430_Interrupt, FALLING);
  timer.run();

  

  if(into_fired)
  {
     pulse_led();
    nfc_irq();
  }

  delay(100);
}

void RF430_Interrupt()            
{
  into_fired = 1;
  detachInterrupt(IRQ);//cancel interrupt
}

void nfc_irq(){
  //clear control reg to disable RF
  nfc.Write_Register(CONTROL_REG, nfc.Read_Register(CONTROL_REG) & ~RF_ENABLE); 
  delay(250);

  //read the flag register to check if a read or write occurred
  flags = nfc.Read_Register(INT_FLAG_REG); 

  //ACK the flags to clear
  nfc.Write_Register(INT_FLAG_REG, EOW_INT_FLAG + EOR_INT_FLAG); 

  if(flags & EOW_INT_FLAG)      //check if the tag was written
  {
    nfc_was_written();
  }
  else if(flags & EOR_INT_FLAG) //check if the tag was read
  {
    nfc_was_read();
  }

  flags = 0;
  into_fired = 0; //we have serviced INT1

  //Configure INTO pin for active low and re-enable RF
  nfc.Write_Register(CONTROL_REG, nfc.Read_Register(CONTROL_REG) | RF_ENABLE); 

  //re-enable INTO
  attachInterrupt(IRQ, RF430_Interrupt, FALLING); 
}

void write_ndef_to_nfc(int fnc, const char* payload, const size_t payload_len){
  Serial.println("Writing NDEF to NFC...");
  NdefRecord records[1];

  switch (fnc){
    case 1:
      Serial.println("Writing text");
      records[0].createText((byte*)payload, payload_len, ENGLISH, true);
      break;
    case 2:
      Serial.println("Writing url"); // NOT CURRENTLY WORKING < AFRAID I FUCK UP
      //records[0].createUri(payload);
      break;
    default:
      Serial.println("Writing borked!");
      return;
  }

  NdefMessage msg(records, sizeof(records)/sizeof(NdefRecord));
  uint16_t msg_length = msg.getByteArrayLength();
  byte message[msg_length];
  msg.toByteArray(message);
  nfc.Write_NDEFmessage(message, msg_length);
}
