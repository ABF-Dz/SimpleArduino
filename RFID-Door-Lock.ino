#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

#define SS_PIN 10
#define RST_PIN 9
#include "pitches.h"

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};
Servo servo1;
// Instance of the class MFRC522
MFRC522 rfid(SS_PIN, RST_PIN);

// Create LCD object : Use 0x27 If 0x3F Doesn't work
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Init array that will store new NUID
byte nuidPICC[4];

// Put Your access NUID Here
byte master[4] = {0x83, 0x6F, 0x16, 0xDD};//83 6F 16 DD

// Pin connected to lock relay signal
int lockOutput = 7;

int greenLED = 3;
int redLED = 4;
int buzzerPin = 8;

void setup() {
Serial.begin (9600);
  servo1.attach(5);
  servo1.write(0);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522

  // Setup LCD with backlight and initialize
  lcd.init();
  lcd.backlight();
  printWelcomeMessage();

  // Set OUTPUT pins
  pinMode(lockOutput, OUTPUT);
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(buzzerPin, OUTPUT);
  
}

void loop() {
  Serial.println(analogRead(A0));

  // Reset the loop if no new card present on the sensor/reader
  if ( ! rfid.PICC_IsNewCardPresent())
    return;

  // Verify if the NUID has been readed
  if ( ! rfid.PICC_ReadCardSerial())
    return;

  // Store NUID into nuidPICC array
  for (byte i = 0; i < 4; i++) {
    nuidPICC[i] = rfid.uid.uidByte[i];
  }

  // Checks whether the scanned tag is authorized
  if (getAccessState(master, nuidPICC) == true) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Open The Door");
    lcd.setCursor(0, 1);
    lcd.print(" Access Granted!");
    // Turn on relay & green LED for 5 seconds
    digitalWrite(greenLED, HIGH);
    servo1.write(90);

    for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
      int noteDuration = 1000 / noteDurations[thisNote];
      tone(8, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
      int pauseBetweenNotes = noteDuration * 1.30;
      delay(pauseBetweenNotes);
    // stop the tone playing:
      noTone(8);
    }
    delay(2000);
    digitalWrite(greenLED, LOW);
    servo1.write(0);
    
    delay(25);  //To fix (LCD, Solenoid) issue
    printWelcomeMessage();

  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(" Retry Again!");
    lcd.setCursor(0, 1);
    lcd.print(" Access Denied!");

    digitalWrite(redLED, HIGH);
    digitalWrite(buzzerPin, HIGH);
    delay(5000);
    digitalWrite(redLED, LOW);
    digitalWrite(buzzerPin, LOW);
    printWelcomeMessage();

  }
  // Halt PICC
  rfid.PICC_HaltA();

  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
  //servo1.write(0);
  printWelcomeMessage();
}

bool getAccessState(byte accessCode[], byte newCode[]) {
  for (byte i = 0; i < 4; i++) {
    if (newCode[i] != accessCode[i]) {
      return false;
    }
  }
  return true;
}

void printWelcomeMessage() {
  lcd.clear();
  lcd.print(" Scan Your Tag!");
  int A=avlb(A0);
  int B=avlb(A1);
  int C=avlb(A2);
  lcd.setCursor(0, 1);
  lcd.print("A=");
  lcd.print(A);
  lcd.print("B=");
  lcd.print(B);
  lcd.print("C=");
  lcd.print(C);
  Serial.println(analogRead(A1));
}
int avlb(char a){
if(analogRead(a)>800){
  return 0;
}else{
  return 1;
}
}
int avlbB(char a){
if(analogRead(a)>750){
  return 0;
}else{
  return 1;
}
}

