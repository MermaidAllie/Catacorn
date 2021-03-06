// C++ demo program for Candy the Catacorn
// https://galaxyallie.space/Candy-the-Catacorn/

#include <Arduino.h>
#include <SPI.h>
#include <Adafruit_DotStar.h>
#include <Adafruit_FreeTouch.h>
#include <adafruit_ptc.h>
#include <samd21_ptc_component.h>

#define NUMSTARS 2
#define STARDATA 7
#define STARCLOCK 8

int moving = 0;
int initialRTouchValue = 0;
int RtouchValue = 0;
int initialLTouchValue = 0;
int LtouchValue = 0;
int n = 30;
int k = 1;
int d = 1;
int i = 30;
int j = 255;
int modeSel = 0;
int light = 0;
int rtime = 0;
bool rainbowActive = false;

// Configure DotStar eyes and Adafruit FreeTouch for ears

Adafruit_DotStar stars = Adafruit_DotStar(NUMSTARS, STARDATA, STARCLOCK, DOTSTAR_BGR);
Adafruit_FreeTouch qt_1 = Adafruit_FreeTouch(A0, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);
Adafruit_FreeTouch qt_2 = Adafruit_FreeTouch(A3, OVERSAMPLE_4, RESISTOR_50K, FREQ_MODE_NONE);

// Pin notes:
// - D0/A2 controls the white lights on the paws
// - D1/A0 is the right ear capacitive touch sensor
// - D3/A3 is the left ear capacitive touch sensor
// - D2/A1 is the first tail section
// - D4/A4 is the second tail section 
// - D13 is the corner of the eyes
// - The eyes are the internal DotStar pin

void setup() {
  Serial.begin(115200); // USB serial is used for diagnostics messages
  pinMode(13, OUTPUT); // Eye corners
  pinMode(0, OUTPUT); // White front paws
  pinMode(2, OUTPUT); // Tail section one
  pinMode(4, OUTPUT); // Tail section two
  stars.setBrightness(191); // 75% max brightness to reduce heat and aging of DotStars
  stars.begin();
  stars.show();
  if (! qt_1.begin()) {
    Serial.println("Failed to begin qt on pin A0");
  }
  if (! qt_2.begin()) {
    Serial.println("Failed to begin qt on pin A3");
  }
  initialRTouchValue = qt_1.measure();
  initialLTouchValue = qt_2.measure();
}

// Capacitive touch detection function for right ear (Candy's right)

bool RcapTouchDetect() {
    Serial.println("\n*** Touch Result ***");
    rtime = millis();
    RtouchValue = qt_1.measure(); 
    Serial.print("Right Ear: "); Serial.print(RtouchValue);
    Serial.print(" (");  Serial.print(millis() - rtime); Serial.println(" ms)");   
    if(RtouchValue > (initialRTouchValue + 100)){
        return true;
    }
    else{
        return false;
    }
}

// Capacitive touch detection function for left ear (Candy's left)

bool LcapTouchDetect() {
    Serial.println("\n*** Touch Result ***");
    rtime = millis();
    LtouchValue = qt_2.measure(); 
    Serial.print("Left Ear: "); Serial.print(LtouchValue);
    Serial.print(" (");  Serial.print(millis() - rtime); Serial.println(" ms)");   
    if(LtouchValue > (initialLTouchValue + 100)){
        return true;
    }
    else{
        return false;
    }
}

// Light show for main loop

void lightShow(int light) {
  if (light == 0)
  {
    n = n + k;
    if (n > 255)
    {
      k = -1;
      n = 255;
    }
    if (n < 30)
    {
      k = 1;
      n = 30;
    }
    for(int pixelSel = 0; pixelSel < NUMSTARS; pixelSel++){
      stars.setPixelColor(pixelSel, 10, 0, ((n / 12) + 3));
    }
    stars.show();
    analogWrite(13, (n / 8));
    analogWrite(0, (n / 20));
    analogWrite(2, (n / 8));
    analogWrite(4, (n / 8));
    delay(10);
  }
  if (light == 1)
  {
    n = n + k;
    if (n > 255)
    {
      k = -1;
      n = 255;
    }
    if (n < 30)
    {
      k = 1;
      n = 30;
    }
    for(int pixelSel = 0; pixelSel < NUMSTARS; pixelSel++){
      stars.setPixelColor(pixelSel, 0, ((n / 12) + 3), 15);
    }
    stars.show();
    analogWrite(13, (n / 8));
    analogWrite(0, (n / 20));
    analogWrite(2, (n / 8));
    analogWrite(4, (n / 8));
    delay(10);
  }
  if(light == 2)
  {
    for(int pixelSel = 0; pixelSel < NUMSTARS; pixelSel++){
      stars.setPixelColor(pixelSel, 5, 0, 5);
    }
    stars.show();
    analogWrite(13, 10);
    analogWrite(0, 255);
    analogWrite(2, 10);
    analogWrite(4, 10);
  }
  if(light == 3)
  {
    for(int pixelSel = 0; pixelSel < NUMSTARS; pixelSel++){
      stars.setPixelColor(pixelSel, 0, 10, 15);
    }
    stars.show();
    analogWrite(13, 10);
    analogWrite(0, 255);
    analogWrite(2, 10);
    analogWrite(4, 10);
  }
}

// Rainbow generator

void rainbowTime () {
  while (rainbowActive == true)
  {
    analogWrite(13, 2);
    analogWrite(0, 2);
    analogWrite(2, 2);
    analogWrite(4, 2);
    for(long rainBowStarColour = 28672; rainBowStarColour < 94207 && rainbowActive == true; rainBowStarColour += 256) 
    {
      uint32_t rainBowSel = stars.gamma32(stars.ColorHSV(rainBowStarColour));
        for(int pixelSel = 0; pixelSel < NUMSTARS && rainbowActive == true; pixelSel++)
        {
            stars.setPixelColor(pixelSel, rainBowSel);
        }
        stars.show();
        if(RcapTouchDetect() == false)
        {
          delay(30);
          if(RcapTouchDetect() == false)
          {
            rainbowActive = false;
          }
        }
        delay(40);
    }
    if(RcapTouchDetect() == false)
    {
      delay(30);
      if(RcapTouchDetect() == false)
      {
        rainbowActive = false;
      }
    }
  }
}

void loop() {
  if(RcapTouchDetect() == true) // Right ear rainbow mode!!!
  {
    delay(50);
    if(RcapTouchDetect() == true)
    {
        rainbowActive = true;
        rainbowTime();
    }      
  }
  if(LcapTouchDetect() == true) // Left ear mode switch
  {
    delay(50);
    if(LcapTouchDetect() == true)
    {
      modeSel++;
      if(modeSel > 3)
      {
        modeSel = 0;
      }
      delay(300);
    }
  };
  lightShow(modeSel);
}