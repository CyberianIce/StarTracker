#include <AccelStepper.h>
#include <EEPROM.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define tonePin 2
#define stepPin 11
#define dirPin 12

String firmware = "1.0.0";
short mode = 0;
float raSiderealRate = 6.610641;
float trackSpeed = raSiderealRate;
float fastSpeed = 2000;
String command = "";
float newRate = raSiderealRate;
AccelStepper RA = AccelStepper(1, stepPin, dirPin);
Adafruit_SSD1306 OLED = Adafruit_SSD1306(128, 64, &Wire);

void setup() {
  trackSpeed = EEPROM.get(0, trackSpeed);
  if(trackSpeed == 'ovf' || trackSpeed == 'nan') {
    trackSpeed = EEPROM.put(0, raSiderealRate);
    trackSpeed = raSiderealRate;
  }
  Serial.begin(57600);
  Serial.setTimeout(10);
  Serial.print("Tracking rate: ");
  Serial.print(trackSpeed, 6);
  Serial.println(" Hz");
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  RA.setPinsInverted(false);
  RA.setMaxSpeed(8000);
  RA.setSpeed(trackSpeed);
  RA.setPinsInverted(true);
  RA.setAcceleration(2000);
  OLED.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  OLED.setTextColor(SSD1306_WHITE);
  OLED.clearDisplay();
  OLED.setCursor(8, 28);
  OLED.print("SiderealTrack ");
  OLED.println(firmware);
  OLED.display();
  OLED.clearDisplay();
  OLED.setCursor(0, 0);
  // introMellody();
}

void loop() {
  
  if(Serial.available() > 0) {
    mode = 0;
    command = Serial.readStringUntil("\n");
    newRate = command.toFloat();
    if(newRate > 0 && newRate <=2000) {
      trackSpeed = newRate;
      EEPROM.put(0, trackSpeed);
      beep(698, 50);
      Serial.print("Tracking rate: ");
      Serial.print(trackSpeed, 6);
      Serial.println(" Hz");
    } else if(command == "RST#") {
      EEPROM.put(0, raSiderealRate);
      trackSpeed = raSiderealRate;
      beep(440, 100);
      beep(698, 120);
      beep(1000, 140);
      Serial.print("Tracking rate: ");
      Serial.print(trackSpeed, 6);
      Serial.println(" Hz");
    } else if(command == "+#") {
      RA.setCurrentPosition(0);
      RA.setSpeed(1000);
      RA.runToNewPosition(1582);
    } else if(command == "-#") {
      RA.setCurrentPosition(0);
      RA.setSpeed(1000);
      RA.runToNewPosition(-1582);
    }
  }

  if(command == "RWD#") {
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
      RA.setPinsInverted(false);
      RA.setSpeed(fastSpeed);
      Serial.print("Rewinding rate: ");
      Serial.print(fastSpeed);
      Serial.println(" Hz");
    }
    mode = 2;
  } else if (command == "FWD#") {
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
      RA.setPinsInverted(true);
      RA.setSpeed(fastSpeed);
      Serial.print("Forwarding rate: ");
      Serial.print(fastSpeed);
      Serial.println(" Hz");
    }
    mode = 3;
  } else {
    if(mode != 1) {
      OLED.clearDisplay();
      OLED.setCursor(58, 0);
      OLED.setTextSize(3);
      OLED.println((char)16);
      OLED.setCursor(42, 24);
      OLED.setTextSize(1);
      OLED.println("SIDEREAL");
      OLED.setCursor(42, 34);
      OLED.println("TRACKING");
      OLED.setCursor(42, 44);
      OLED.println(trackSpeed, 6);
      OLED.display();
      RA.setPinsInverted(true);
      RA.setSpeed(trackSpeed);
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
