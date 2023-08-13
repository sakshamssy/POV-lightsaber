#include "File.h"
#include<Wire.h>
#include <MPU6050_light.h>

MPU6050 mpu(Wire);
unsigned long timer = 0;

#define PI 3.1415926535897932384626433832795

#include <FastLED.h>
#define NUM_LEDS 144
#define DATA_PIN 32
#define CLOCK_PIN 13
CRGB leds[NUM_LEDS];

const int pixelCount = 144;
int arr[pixelCount][2*pixelCount];
float angle = 0;
int x,y;
int AdjustedPixelCount;
float angleRadian = angle*PI/180;
int average_r, average_g, average_b;
int currentImage = 12;
unsigned char image[128*128][3];

  bool loadCurrentImage()
{
  char filename[32];
  sprintf(filename, "/spiffs/image%d.bin", currentImage);
  Serial.println(filename);
  if(!readFromFile(filename, image[0], 128 * 128 * 3))
  {
    for(int y = 0; y < 128; y++){
      for(int x = 0; x < 128; x++)
      {
        image[y * 128 + x][0] = 0;//x * 2;
        image[y * 128 + x][1] = 0;//y * 2;
        image[y * 128 + x][2] = 0;//254 - x * 2;
      }  
    }  
    return false;
  }
  return true;
}


void setup() {

Wire.begin();
  Serial.begin(9600);
  byte status = mpu.begin();
  Serial.print(F("MPU6050 status: "));
  Serial.println(status);
  while(status!=0){ } // stop everything if could not connect to MPU6050
  
  Serial.println(F("Calculating offsets, do not move MPU6050"));
  delay(1000);
  // mpu.upsideDownMounting = true; // uncomment this line if the MPU6050 is mounted upside-down
  mpu.calcOffsets(); // gyro and accelero
  Serial.println("Done!\n");
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(100);
  FastLED.clear();
  
  
  Serial.println();
  initFileSystem();
  loadCurrentImage();
  //random_generate();
  //delay(10);

  

  
}

void loop() {
mpu.update();
  
angle = mpu.getAngleZ();
angleRadian = ((angle+90))*PI/180;
AdjustedPixelCount = 64/(cos(angleRadian));
//Serial.println((angle+90)%360);
  //Serial.println("Done");
  for(int i=0;i<pixelCount;i++){
    x = 64+(cos(angleRadian) * i);
    y = 127-(sin(angleRadian) * i);
    if (i <=AdjustedPixelCount){
      average_r = (image[y*128 +x][0] + image[(y+1)*128 + x][0] + image[y*128 + x+1][0] + image[(y+1)*128 + x+1][0])/4;
      average_g = (image[y*128 +x][1] + image[(y+1)*128 + x][1] + image[y*128 + x+1][1] + image[(y+1)*128 + x+1][1])/4;
      average_b = (image[y*128 +x][2] + image[(y+1)*128 + x][2] + image[y*128 + x+1][2] + image[(y+1)*128 + x+1][2])/4;
      //leds[i].setRGB( image[y*128 + x][0], image[y*128 + x][1], image[y*128 + x][2]);
      leds[i].setRGB( average_r, average_g, average_b);  
    }else {
      leds[i].setRGB( 0, 0, 0);
    }    
  }
  FastLED.show();
  
}
