#include <AccelStepper.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define rewindPin 9
#define forwardPin 10
#define stepPin 11
#define dirPin 12

short mode = 0;
float siderealRate = 7.501964;
float fastSpeed = 4000;

AccelStepper STEPPER = AccelStepper(1, stepPin, dirPin);
Adafruit_SSD1306 OLED = Adafruit_SSD1306(128, 64, &Wire);

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(forwardPin, INPUT_PULLUP);
  pinMode(rewindPin, INPUT_PULLUP);
  STEPPER.setPinsInverted(false);
  STEPPER.setMaxSpeed(8000);
  STEPPER.setSpeed(siderealRate);
  OLED.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  OLED.setTextColor(SSD1306_WHITE);
  OLED.clearDisplay();
  OLED.setCursor(0, 0);
  Serial.println("RDY");
}

void loop() {



  uint8_t rewindPin_mask = digitalPinToBitMask(rewindPin);
  volatile uint8_t *rewindPin_port = portInputRegister(digitalPinToPort(rewindPin));
  uint8_t rewindPinState = (*rewindPin_port & rewindPin_mask) != 0;

  uint8_t forwardPin_mask = digitalPinToBitMask(forwardPin);
  volatile uint8_t *forwardPin_port = portInputRegister(digitalPinToPort(forwardPin));
  uint8_t forwardPinState = (*forwardPin_port & forwardPin_mask) != 0;
  
  if (!rewindPinState) {
    
     if (mode != 2) {
      digitalWrite(LED_BUILTIN, HIGH);
      OLED.clearDisplay();
      OLED.setCursor(40, 6);
      OLED.setTextSize(4);
      OLED.print((char)17);
      OLED.println((char)17);
      OLED.setCursor(48, 38);
      OLED.setTextSize(1);
      OLED.println("REWIND");
      OLED.display();
      STEPPER.setPinsInverted(false);
      STEPPER.setSpeed(fastSpeed);
      Serial.println("REWIND");
    }
    mode = 2;
    STEPPER.runSpeed();

  } else if (!forwardPinState) {

    if (mode != 3) {
      digitalWrite(LED_BUILTIN, HIGH);
      OLED.clearDisplay();
      OLED.setCursor(40, 6);
      OLED.setTextSize(4);
      OLED.print((char)16);
      OLED.println((char)16);
      OLED.setCursor(42, 38);
      OLED.setTextSize(1);
      OLED.println("FORWARD");
      OLED.display();
      STEPPER.setPinsInverted(true);
      STEPPER.setSpeed(fastSpeed);
      Serial.println("FORWARD");
    }
    mode = 3;
    STEPPER.runSpeed();

  } else {

    if (mode != 1) {
      digitalWrite(LED_BUILTIN, LOW);
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
      STEPPER.setPinsInverted(true);
      STEPPER.setSpeed(siderealRate);
      Serial.println("SIDEREAL");
    }
    mode = 1;

  }

  STEPPER.runSpeed();

}
