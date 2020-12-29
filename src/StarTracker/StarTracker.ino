#include <AccelStepper.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define tonePin 2
#define rewindPin 9
#define forwardPin 10
#define stepPin 11
#define dirPin 12

String firmware = "1.0.0";
short mode = 0;
float raSiderealRate = 6.610641;
float fastSpeed = 2000;
AccelStepper RA = AccelStepper(1, stepPin, dirPin);
Adafruit_SSD1306 OLED = Adafruit_SSD1306(128, 64, &Wire);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(forwardPin, INPUT_PULLUP);
  pinMode(rewindPin, INPUT_PULLUP);
  RA.setPinsInverted(false);
  RA.setMaxSpeed(8000);
  RA.setSpeed(raSiderealRate);
  OLED.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  OLED.setTextColor(SSD1306_WHITE);
  OLED.clearDisplay();
  OLED.setCursor(8, 28);
  OLED.print("SiderealTrack ");
  OLED.println(firmware);
  OLED.display();
  OLED.clearDisplay();
  OLED.setCursor(0, 0);
  introMellody();
}

void loop() {

  uint8_t rewindPin_mask = digitalPinToBitMask(rewindPin);
  volatile uint8_t *rewindPin_port = portInputRegister(digitalPinToPort(rewindPin));
  uint8_t rewindPinState = (*rewindPin_port & rewindPin_mask) != 0;

  uint8_t forwardPin_mask = digitalPinToBitMask(forwardPin);
  volatile uint8_t *forwardPin_port = portInputRegister(digitalPinToPort(forwardPin));
  uint8_t forwardPinState = (*forwardPin_port & forwardPin_mask) != 0;
 
  if(!rewindPinState) {
     if (mode != 2) {
      OLED.clearDisplay();
      OLED.setCursor(40, 6);
      OLED.setTextSize(4);
      OLED.print((char)17);
      OLED.println((char)17);
      OLED.setCursor(48, 38);
      OLED.setTextSize(1);
      OLED.println("REWIND");
      OLED.display();
      RA.setPinsInverted(true);
      RA.setSpeed(fastSpeed);
    }
    mode = 2;
  } else if(!forwardPinState) {
    if (mode != 3) {
      OLED.clearDisplay();
      OLED.setCursor(40, 6);
      OLED.setTextSize(4);
      OLED.print((char)16);
      OLED.println((char)16);
      OLED.setCursor(42, 38);
      OLED.setTextSize(1);
      OLED.println("FORWARD");
      OLED.display();
      RA.setPinsInverted(false);
      RA.setSpeed(fastSpeed);
    }
    mode = 3;
  } else {
    if(mode != 1) {
      OLED.clearDisplay();
      OLED.setCursor(58, 6);
      OLED.setTextSize(3);
      OLED.println((char)16);
      OLED.setCursor(42, 32);
      OLED.setTextSize(1);
      OLED.println("SIDEREAL");
      OLED.setCursor(42, 42);
      OLED.println("TRACKING");
      OLED.display();
      RA.setPinsInverted(false);
      RA.setSpeed(raSiderealRate);
    }
    mode = 1;
  }
  RA.runSpeed();
}

void beep(int note, int duration) {
  tone(tonePin, note, duration);
  delay(duration);
  noTone(tonePin);
  delay(25);
}

void introMellody() {
  beep(440, 500);
  beep(440, 500);    
  beep(440, 500);
  beep(349, 350);
  beep(523, 150);  
  beep(440, 500);
  beep(349, 350);
  beep(523, 150);
  beep(440, 650);
  delay(300);
  beep(659, 500);
  beep(659, 500);
  beep(659, 500);  
  beep(698, 350);
  beep(523, 150);
  beep(415, 500);
  beep(349, 350);
  beep(523, 150);
  beep(440, 650);
}
