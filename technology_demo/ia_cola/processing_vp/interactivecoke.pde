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
String serialport = "/dev/ttyACM0";
import processing.serial.*;
import ddf.minim.*;

Serial my_port;
Minim minim;
AudioPlayer player;

int value = 0;
int time;
int wait = 5;

String serialdata;

boolean idle = false;
boolean tick;

int i = 0;

int num_frames = 17;
PImage[] out_img = new PImage[num_frames];

void setup() {
  
   // List all the available serial ports
  println(Serial.list());
  // Open the port you are using at the rate you want:
  my_port = new Serial(this, serialport, 115200);
  
 
  
  
  minim = new Minim(this);
  player = minim.loadFile("open2.mp3");
  size(1366, 768);
  // size(720, 450);
  background(0);
  smooth();
  imageMode(CENTER);
  //  noLoop();
  
  // Load and resize all images at startup to increase frame rate
  for (int i = 0; i < num_frames; i++) {
    String image_name = (i+1) + ".png";
    out_img[i] = loadImage(image_name);
    out_img[i].resize(0, 1800);
  }
 
  time = millis();//store the current time
}

void draw() {
  
  
  if ( my_port.available() > 0) 
  {  // If data is available,

  serialdata = my_port.readStringUntil('\n');         // read it and store it in val
  println(serialdata);
    if (serialdata.indexOf('_') > 0){
      idle = false;
    }
  }
  translate(500, height/2);
  rotate(-HALF_PI);

  /*
  if (millis() - time >= wait) {
    tick = !tick;//if it is, do something
    time = millis();//also update the stored time
  }*/

  if (idle) {
    image(out_img[0], 0, 0);
  } else {
    player.play();
    image(out_img[i], 0, 0);
    println("Redraw,", i);
    i++;
    
    if (i == num_frames) {
      println("Last image");
      delay_ms(2000);
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

void delay_ms(int delay) {
  
  int time = millis();
  while(millis() - time <= delay);
}
