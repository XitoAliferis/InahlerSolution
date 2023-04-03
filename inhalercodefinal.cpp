#include "LedControl.h"

LedControl lc = LedControl(2, 4, 3, 1);  // Pins: DIN,CLK,CS, # of Display connected

const int buttonPin = 7;
const int buzzerPin = 6;
const int canisterPin = 5;

int puffs = 20;
int initialpuffs;

unsigned long buttonStart = 0;
int canisterState = 0;
int buttonState = 0;

byte rows[8] = {
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000,
  B00000000
};

void setup() {
  initialpuffs = puffs;
  lc.shutdown(0, false);  // Wake up displays
  lc.shutdown(1, false);
  lc.setIntensity(0, 5);  // Set intensity levels
  lc.setIntensity(1, 5);
  lc.clearDisplay(0);  // Clear Displays
  lc.clearDisplay(1);
  pinMode(buzzerPin, OUTPUT);
  pinMode(canisterPin, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);
  Serial.begin(4800); 
}

void loop() {  
  buttonState = digitalRead(buttonPin);
  canisterState = digitalRead(canisterPin);
  if (canisterState == LOW) {
    puffs--;
    tone(buzzerPin,1000); //buzz every click
    delay(100);
    noTone(buzzerPin);
    
    Serial.println(puffs);
    if (((double)puffs/initialpuffs) <= 0.20 && ((double)puffs/initialpuffs) > 0.10){ // 20-10 percent show full LED
      full();
    }
    if (((double)puffs/initialpuffs) <= 0.10 && ((double)puffs/initialpuffs) > 0.05){// 10-5 percent show half led
      half();
    }
    if (((double)puffs/initialpuffs) <= 0.05 && ((double)puffs/initialpuffs) > 0){// 5% or lower show and X twice and buzz 3 times
      xfunction();
    }
    if (((double)puffs/initialpuffs) <= 0){// 0 puffs buzz for 3 seconds
        tone(buzzerPin,1000);
        delay(2900);
        noTone(buzzerPin);
        delay(100);
    }
    delay(1000);
    rrows();
  }

  if (buttonState == HIGH){
    if (buttonStart == 0) {
      buttonStart = millis();
    }
    if (millis() - buttonStart > 5000 && millis() - buttonStart < 6000){
      puffs = initialpuffs;
      gradient();
      for (int i = 0; i < 2; i++){
        tone(buzzerPin,1000);
        delay(100);
        noTone(buzzerPin);
        delay(100);
      }
      SetPuffs(initialpuffs); // set puffs
      Serial.println("worked?");
      for (int i = 0; i < 2; i++){
        tone(buzzerPin,1000);
        delay(100);
        noTone(buzzerPin);
        delay(100);
      }
      buttonStart = 0;
    }
  }
  else {
    buttonStart = 0;
  }
}

void full(){
    for (int i = 0; i < 8; i++) {
    rows[i] = B11111111;
    lc.setRow(0, i, rows[i]);
  }
}

void half(){
    for (int i = 0; i < 5; i++) {
    rows[i] = B11111111;
    lc.setRow(0, i, rows[i]);
  }
}

void xfunction(){
      byte xrows[8] = {
    B10000001,
    B01000010,
    B00100100,
    B00011000,
    B00011000,
    B00100100,
    B01000010,
    B10000001
  };

   for (int i = 0; i < 8; i++) {
    lc.setRow(0, i, xrows[i]);
  }
}

void updateDisplay() {
  int rowsToShow = 8 * (((double)puffs) / ((double)initialpuffs*0.2));
      Serial.println(puffs);
      Serial.println((double)puffs*0.2);
  for (int i = 0; i < 8; i++) {
    if (i >= rowsToShow) {
      rows[i] = B00000000;
    }
    else {
      rows[i] = B11111111;
    }
    lc.setRow(0, i, rows[i]);
  }
}

void rrows(){
  for (int i = 7; i >= 0; i--) {
    rows[i] = B00000000;
    lc.setRow(0, i, rows[i]);
  }
}

void gradient(){
  rrows();
  for (int i = 0; i < 8; i++) {
    rows[i] = B11111111;
    lc.setRow(0, i, rows[i]);
    delay(100);
  }
  for (int i = 7; i >= 0; i--) {
    rows[i] = B00000000;
    lc.setRow(0, i, rows[i]);
    delay(100);
  }
}


void SetPuffs(int original) { // set puffs with button
  int buttonClicks = 0;
  unsigned long buttonHoldStart = 0;
  while (true) {
    buttonState = digitalRead(buttonPin);
    if (buttonState == HIGH && buttonHoldStart < 350) { // if clicked
      buttonHoldStart = millis();
      buttonClicks++; //add 1
      tone(buzzerPin,1000); //buzz for button offset time
      delay(100);
      noTone(buzzerPin);
      delay(250);
    }
    if (buttonState == LOW){
      buttonHoldStart = 0;
    }
    if (buttonState == HIGH && buttonClicks == 1 && millis() - buttonHoldStart > 3000) { // reset original
      puffs = original;
      Serial.println(puffs);
      full();
      delay(1000);
      rrows();
      return; // exit the function
    }
    if (buttonState == HIGH && buttonClicks > 1 && millis() - buttonHoldStart > 3000) { // if held for 3 seconds
      buttonClicks--;
      puffs = 5 * buttonClicks; // 50 x Number of clicks (e.g. 5 clicks = 250 puffs)
      Serial.println(puffs);
      initialpuffs = puffs;
      full();
      delay(1000);
      rrows();
      return; // exit the function
    }
  }
}
