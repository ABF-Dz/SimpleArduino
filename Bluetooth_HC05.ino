// NOTE: change the Serial to other Serial/Software Serial if you connects Bluetooth module to other pins
#define LED_PIN 13
#include "pitches.h"

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};
int SOUND;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT); // set the digital pin as output:
}

void loop() {
  if (Serial.available()) { // if there is data comming
    String command = Serial.readStringUntil('\n'); // read string until meet newline character

    if (command == "OFF") {
      digitalWrite(LED_PIN, LOW);  // turn off LED
      Serial.println("LED is turned OFF");
      SOUND=0; 
      // reports action to smartphone app

    } else if (command == "ON") {
      digitalWrite(LED_PIN, HIGH); // turn on LED
      SOUND=1;
      if (SOUND==1){
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
      SOUND+=1;
      }
      Serial.println("LED is turned ON"); // reports action to smartphone app
    }
  }
}