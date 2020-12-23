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

short mode = 0;
// float raSiderealRate = 7.501964;
float raSiderealRate = 6.610641;
float raTrackingSpeed = raSiderealRate;
float fastSpeed = 2000;
unsigned long previousMillis = 0;
const long interval = 100;

AccelStepper STEPPER = AccelStepper(1, stepPin, dirPin);
Adafruit_SSD1306 OLED = Adafruit_SSD1306(128, 64, &Wire);

void setup() {
  Serial.begin(57600, SERIAL_8N1);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  pinMode(forwardPin, INPUT_PULLUP);
  pinMode(rewindPin, INPUT_PULLUP);
  STEPPER.setPinsInverted(false);
  STEPPER.setMaxSpeed(8000);
  STEPPER.setSpeed(raTrackingSpeed);
  OLED.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  OLED.setTextColor(SSD1306_WHITE);
  OLED.clearDisplay();
  OLED.setCursor(42, 28);
  OLED.println("Welcome");
  OLED.display();
  Serial.setTimeout(5);
  tone(tonePin, 960);
  delay(350);
  tone(tonePin, 1260);
  delay(400);
  tone(tonePin, 1680);
  delay(500);
  noTone(tonePin);
  OLED.clearDisplay();
  OLED.setCursor(0, 0);
  Serial.println("INITIALIZED#");
}

void loop() {

  unsigned long currentMillis = millis();

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
      STEPPER.setPinsInverted(false);
      raTrackingSpeed = fastSpeed;
    }
    mode = 2;
    STEPPER.runSpeed();
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
      STEPPER.setPinsInverted(true);
      raTrackingSpeed = fastSpeed;
    }
    mode = 3;
    STEPPER.runSpeed();
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
    }
    mode = 1;
    

    if(Serial.available() > 0) {
      String opcode = Serial.readStringUntil('#');
      boolean validOpcode = true;
      if(opcode=="CONNECT") {
        raTrackingSpeed = raSiderealRate;
        digitalWrite(LED_BUILTIN, LOW);
      } else if (opcode=="DISCONNECT") {
        raTrackingSpeed = raSiderealRate;
        digitalWrite(LED_BUILTIN, LOW);
      } else if(opcode=="RA0") {
        STEPPER.setPinsInverted(true);
        raTrackingSpeed = raSiderealRate;
      } else if(opcode=="RA+") {
        raTrackingSpeed = raSiderealRate * 1.5;
        digitalWrite(LED_BUILTIN, HIGH);
      } else if(opcode=="RA-") {
        raTrackingSpeed = raSiderealRate * 0.5;
        digitalWrite(LED_BUILTIN, HIGH);
      } else if(opcode=="DEC0") {
        // TO BE IMPLEMENTED
      } else if(opcode=="DEC+") {
        // TO BE IMPLEMENTED
      } else if(opcode=="DEC-") {
        // TO BE IMPLEMENTED
      } else {
        validOpcode = false;
      }
      if(validOpcode) {
        Serial.println("OK#");
      }
    } else {
      if (raTrackingSpeed != fastSpeed) {
        if(currentMillis - previousMillis >= interval) {
          previousMillis = currentMillis;
          raTrackingSpeed = raSiderealRate;
          STEPPER.setPinsInverted(true);
          digitalWrite(LED_BUILTIN, LOW);
        }
      } else {
        raTrackingSpeed = raSiderealRate;
        STEPPER.setPinsInverted(true);
        digitalWrite(LED_BUILTIN, LOW);
      }
    }

  }
  STEPPER.setSpeed(raTrackingSpeed);
  STEPPER.runSpeed();
}
