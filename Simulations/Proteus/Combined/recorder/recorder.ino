#include "LibsANDdefs.h"

char mode = '8';
/*  mode = 1 for record
        press '*' to stop recording
    mode = 2 for display values
        enter track number and press '*'
        enter '*' to stop playing
    mode = * for delete all files
        enter the response and press '*'
    mode = 0 for delete a specific file
        enter track number and press '*'
        enter '*' to delete track
    mode = # for display files
    mode = 8 for pause state
*/

LCDScreen lcd;
byte freqScal=0;
int realVals[12] = {186, 102, 0, 359, 308, 248, 473, 439, 399, 554, 529, 501};
char keys[12] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '0', '#'};
//long t;

//Initializing things
void setup() {
  
  //CONFIGURE ANALOD READ FOR FASTER READINGS
  ADCSRA |= (1<<ADPS2);
  ADCSRA &= ~(1<<ADPS1);
  ADCSRA &= ~(1<<ADPS0);

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
  if (mode == '1') {   
    record();   
    mode = '8';   
  }
  
  //################################# PLAY MODE ###################################
  if (mode == '2') {      
    playTrack();   
    mode = '8';
  }
  
  //############################### DELETE ALL MODE ###############################
  if (mode == '*') {
    clrDisplay("Delete all tracks ?");
    secondLine("Yes-1 No-2");

    if (getKeyInput() == "1.WAV") {
      fileOperation(1);
      clrDisplay("Tracks deleted !");
      delay(1000);
    }
    else {
      clrDisplay("Leaving...");
      delay(1000);
    }
    
    mode = '8';
  }
  //########################### DISPLAY TRACKS MODE ################################
  if (mode == '#') {
    fileOperation(3);   
    mode = '8';
  }
  //########################### DELETE FILE MODE ###################################
  if (mode == '0') { 
    fileOperation(2);
    clrDisplay("Leaving...");
    delay(1000);  
    mode = '8';
  }
  //############################ PAUSE MODE ########################################
  if (mode == '8') {   
    
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
