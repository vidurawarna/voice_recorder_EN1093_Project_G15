#include "LibsANDdefs.h"

char mode = 'i';
/*  mode = 'r' for record
        press 'record' to stop recording
    mode = 'p' for player mode
        play the track by skipping and press 'play'
        press 'stop' to stop playing
    mode = 'i' for pause state
*/

LCDScreen lcd(0x20);
byte freqScal = 0;
String fname_temp;

int realVals[8] = {501, 0, 248, 334, 522,  78, 294, 429};
char keys[8] = {'r', '<', 'p', '>', 's',  'd', 'm', 'o'};
//long t;

//Initializing things
void setup() {

  //CONFIGURE ANALOD READ FOR FASTER READINGS
  ADCSRA |= (1 << ADPS2);
  ADCSRA &= ~(1 << ADPS1);
  ADCSRA &= ~(1 << ADPS0);

  pinMode(pot, INPUT);
  pinMode(keypadPin, INPUT);

  //CONFIGURING PORTD FOR OUTPUT
  for ( int i = 0; i < 8; i++) {
    pinMode(i, OUTPUT);
  }

 //Serial.begin(9600);

  lcd.begin();
  firstLine("Starting...");
  delay(1000);

  if (!SD.begin(10)) {
    clrDisplay("SD card Error!");
    while (1);
  }

  secondLine("Welcome !!");

  delay(1000);
}


void loop() {

  //################################ KEY INPUT ####################################
  char key_input = keyInput();
  if (key_input) {
    mode = key_input;
  }

  //################################ RECORD MODE ##################################
  if (mode == 'r') {
    record();
    mode = 'i';
  }

  //################################# PLAYER MODE ###################################
  if (mode == 'p') {
    player();
    mode = 'i';
  }

  //############################ PAUSE MODE ########################################
  if (mode == 'i') {

    clrDisplay("Voice Recorder");

    while (true) {
      char key_input = keyInput();
      if (key_input) {
        mode = key_input;
        break;
      }
    }
  }

}
