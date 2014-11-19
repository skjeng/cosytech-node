/*
*   ____               _____         _     
*  / ___|___  ___ _   |_   _|__  ___| |__  
* | |   / _ \/ __| | | || |/ _ \/ __| '_ \ 
* | |__| (_) \__ \ |_| || |  __/ (__| | | |
*  \____\___/|___/\__, ||_|\___|\___|_| |_|
*                 |___/                    
*
* CosyTech Skunkworks - Active NFC Only
* By Joakim Skjefstad (joakim@cosytech.net)
* Based on example code by Ten Wong
*
* Dependencies:
* https://github.com/awong1900/RF430CL330H_Shield 
* 
* Pins used:
* I2C - A4, A5
* /RST - D4
* INT0 - D3
* VCC - 3.3v
* GND - GND
*/


/* Copyright 2013-2014 Ten Wong, wangtengoo7@gmail.com  
*  https://github.com/awong1900/RF430CL330H_Shield 
*  More info : http://www.elecfreaks.com
*/
//the I2C part of this code is borrowed from Adafruit_NFCShield_I2C
//link to original https://github.com/adafruit/Adafruit_NFCShield_I2C

//  RF430 User Address Map
//  -----------------------------------------
//  Address     | Size  | Description       |
//  -----------------------------------------
//  0xFFFE      | 2B    | Control Register  |
//  0xFFFC      | 2B    | Status Register   |
//  0xFFFA      | 2B    | Interrupt Enable  |
//  0xFFF8      | 2B    | Interrupt Flags   |
//  0xFFF6      | 2B    | CRC Result        |
//  0xFFF4      | 2B    | CRC Length        |
//  0xFFF2      | 2B    | CRC Start Address |
//  0xFFF0      | 2B    | Comm WD Ctrl Reg  |
//  -----------------------------------------
//  0x0000 -    | 2kB   | NDEF App Memory   |
//    0x07FF    |       |                   |
//
//
//
//                                /|\  /|\    (Host/Tester)
//                   RF430        10k  10k     Arduino 2560
//                  (Slave)        |    |        Master
//             _________________   |    |   _________________
//            |              SDA|<-|----+->|P20              |
//            |                 |  | I2C   |                 |
//            |              SCL|<-+------>|P21              |
//            |                 |          |                 |
//      GND<--|E(2-0)       /RST|<---------|P4               |
//            |             INTO|--------->|P3(INT1)         |
//            |                 |          |                 |
//            |                 |          |                 |
//            |                 |          |                 |
//            |_________________|          |_________________|
//
//******************************************************************************

#include <Wire.h>

#include <RF430CL330H_Shield.h>
#include <NdefMessage.h>
#include <NdefRecord.h>

#define IRQ   (1)   //External interrupt 1 is found on pin 3
#define RESET (4)

#define BAUDRATE 115200

const int led = 13;

RF430CL330H_Shield nfc(IRQ, RESET);
volatile byte into_fired = 0;
uint16_t flags = 0;

String target_url = "http://cosytech.net";

void nfc_was_read(){
  Serial.println(F("Tag was read"));
  digitalWrite(led, LOW);
  delay(100);
  digitalWrite(led, HIGH);
}
void nfc_was_written(){
  Serial.println(F("Tag was written"));
  digitalWrite(led, LOW);
  delay(200);
  digitalWrite(led, HIGH);
}

void shutdown(){
  Serial.println(F("D"));
  for(;;)
    ;
}

void setup(void) 
{
  
  Serial.begin(BAUDRATE);
  Serial.println(F("CosyTech Active NFC started"));
  nfc.begin();
  NdefRecord records[1];
  records[0].createUri(target_url);
  NdefMessage msg(records, sizeof(records)/sizeof(NdefRecord));
  uint16_t msg_length = msg.getByteArrayLength();
  byte message[msg_length];
  msg.toByteArray(message);
  nfc.Write_NDEFmessage(message, msg_length);
  nfc_irq();
}

void loop(void) 
{
  attachInterrupt(IRQ, RF430_Interrupt, FALLING);
  
  if(into_fired)
  {
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
  delay(750);

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

