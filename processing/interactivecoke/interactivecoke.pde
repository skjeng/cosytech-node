/*
*   ____               _____         _     
*  / ___|___  ___ _   |_   _|__  ___| |__  
* | |   / _ \/ __| | | || |/ _ \/ __| '_ \ 
* | |__| (_) \__ \ |_| || |  __/ (__| | | |
*  \____\___/|___/\__, ||_|\___|\___|_| |_|
*                 |___/                    
*
* CosyTech Skunkworks - Interactive Advertisement Demo
* By Joakim Skjefstad (joakim@cosytech.net)
*
* Dependencies:
* An Active NFC tag that answers on serial baudrate 115200
* 
* open1.aif https://www.freesound.org/people/JasonElrod/sounds/85471/
* open2.mp3 https://www.freesound.org/people/morgantj/sounds/58846/
* open3.mp3 http://www.freesound.org/people/kiddpark/sounds/201159/
*/

// Remember to edit
String serialport = "/dev/tty.usbmodem1a1221";



import processing.serial.*;
import ddf.minim.*;

Serial myPort;
Minim minim;
AudioPlayer player;

int value = 0;
int time;
int wait = 1000;

String serialdata;



boolean idle = true;
boolean tick;

int i = 0;

PImage out_img;
String[] image = {
  "idle.png", "s1.png", "s2.png", "s3.png", "s4.png", "empty.png"
};

void setup() {
   // List all the available serial ports
  println(Serial.list());
  // Open the port you are using at the rate you want:
  myPort = new Serial(this, serialport, 115200);
  
  minim = new Minim(this);
  player = minim.loadFile("open2.mp3");
  size(1440, 900);
  background(0);
  smooth();
  imageMode(CENTER);
  //  noLoop();

  time = millis();//store the current time
}

void draw() {
  if ( myPort.available() > 0) 
  {  // If data is available,

  serialdata = myPort.readStringUntil('\n');         // read it and store it in val
  println(serialdata);
    if (serialdata.indexOf('_') > 0){
      idle = false;
    }
  } 
  translate(500, height/2);
  rotate(-HALF_PI);

  if (millis() - time >= wait) {
    tick = !tick;//if it is, do something
    time = millis();//also update the stored time
  }

  if (idle) {
    out_img = loadImage(image[0]);
    out_img.resize(0, 1800);
    image(out_img, 0, 0);
  } else {
    player.play();
    out_img = loadImage(image[i]);
    out_img.resize(0, 1800);
    image(out_img, 0, 0);
    println("Redraw,", i);
    i++;
    
    if (i == image.length) {
      println("Last image");
      delay(8000);
      i = 0;
      idle = true;
      player.rewind();
    }
  }
}

void keyPressed() {
  println("Key pressed, activating");
  idle = false;
}
void delay(int delay)
{
  int time = millis();
  while(millis() - time <= delay);
}
