#include <LiquidCrystal.h>
#include <Servo.h>

// LDR jutikliai:
const int ldr1Pin = A0;
const int ldr2Pin = A1;

// LED:
const int led1Pin = 3;
const int led2Pin = 4;

// Įvažiavimo mygtukas:
const int buttonPin = 5;

// Buzzer(piezo):
const int buzzerPin = 6;

// Servo:
const int servoPin = 9;

LiquidCrystal lcd(10, 11, 12, 13, A2, A3); // LCD: RS, E, D4, D5, D6, D7
Servo gateServo;

// LDR histerezė:
const int occupiedThreshold = 300;
const int freeThreshold = 400;

// Servo pasisukimo kampas:
const int gateClosed = 0;
const int gateOpen = 90;

// Dabartinė vietų būsena:
bool occupied1 = false;
bool occupied2 = false;

void setup() {
  Serial.begin(9600);

  pinMode(led1Pin, OUTPUT);
  pinMode(led2Pin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);

  // Servo:
  gateServo.attach(servoPin);
  gateServo.write(gateClosed);

  // LCD:
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
  lcd.print("SMART PARKING");
  lcd.setCursor(0, 1);
  lcd.print("STARTING...");

  delay(1500);
  lcd.clear();
}

void loop() {
  int ldr1 = analogRead(ldr1Pin);
  int ldr2 = analogRead(ldr2Pin);

  if (ldr1 < occupiedThreshold) {
    occupied1 = true;
  }  else if (ldr1 > freeThreshold) {
    occupied1 = false;
  }

  if (ldr2 < occupiedThreshold) {
    occupied2 = true;
  } else if (ldr2 > freeThreshold) {
    occupied2 = false;
  }

  // LED indikatoriai:
  digitalWrite(led1Pin, occupied1 ? LOW : HIGH);
  digitalWrite(led2Pin, occupied2 ? LOW : HIGH);

  // Skaičiuojame laisvas vietas:
  int freeSpaces = 0;

  if (!occupied1) {
    freeSpaces++;
  }

  if (!occupied2) {
    freeSpaces++;
  }

  lcd.setCursor(0, 0);
  lcd.print("SMART PARKING   ");
  lcd.setCursor(0, 1);
  lcd.print("FREE: ");
  lcd.print(freeSpaces);
  lcd.print("          ");

  // Serial monitor output:
  Serial.print("LDR1: ");
  Serial.print(ldr1);
  Serial.print(occupied1 ? " [UZIMTA]" : " [LAISVA]");
  Serial.print("   LDR2: ");
  Serial.print(ldr2);
  Serial.print(occupied2 ? " [UZIMTA]" : " [LAISVA]");
  Serial.print("   FREE: ");
  Serial.println(freeSpaces);

  // Įvažiavimo mygtukas:
  if (digitalRead(buttonPin) == LOW) {
    delay(40); 
    
    if (digitalRead(buttonPin) == LOW) {
      if (freeSpaces > 0) {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("WELCOME!");
        lcd.setCursor(0, 1);
        lcd.print("GATE OPEN");

        // Trumpas signalas:
        tone(buzzerPin, 1200, 200);
        // Atidarome vartus:
        gateServo.write(gateOpen);
        delay(3000);

        // Uždarome vartus:
        gateServo.write(gateClosed);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("GATE CLOSED");
        delay(1000);
      }

      else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("PARKING FULL!");
        lcd.setCursor(0, 1);
        lcd.print("NO FREE SPACE");

        // Perspėjimo signalas:
        tone(buzzerPin, 500, 300);
        delay(400);
        tone(buzzerPin, 500, 300);
        delay(400);
        tone(buzzerPin, 500, 300);
        delay(1500); // Servo NEJUDA
      }

      // Laukiame, kol bus atleistas nuspaustas mygtukas:
      while (digitalRead(buttonPin) == LOW) {
        delay(10);
      }

      lcd.clear();
    }
  }

  delay(100);
}
