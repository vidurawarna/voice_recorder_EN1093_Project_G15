#include "LibsANDdefs.h"

unsigned long st,t;
byte pot_Read,fsDelayin =400,fsDelayout= 400;
const byte chipSelect = 10;
File test_File, test_File2;
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

//setting up the keypad
int realVals[12] = {186, 102, 0, 359, 308, 248, 473, 439, 399, 554, 529, 501};
char keys[12] = {'1', '2', '3', '4', '5', '6', '7', '8', '9', '*', '0', '#'};
//const char r = 4;
//const char c = 3;
//char keys[r][c] = {
//  {'1', '2', '3'},
//  {'4', '5', '6'},
//  {'7', '8', '9'},
//  {'*', '0', '#'}
//};
//
//char rPins[r] = {r1, r2, r3, r4};
//char cPins[c] = {c1, c2, c3};
//
//Keypad keypad = Keypad(makeKeymap(keys), rPins, cPins, r, c);
LCDScreen lcd(0x20, 16, 2);

//Initializing things
void setup() {

  pinMode(pot, INPUT);
  pinMode(keypadPin, INPUT);
 // pinMode(buttonPin, INPUT_PULLUP);
  
  //CONFIGURING PORTD FOR OUTPUT
  for ( int i = 0; i < 8; i++) {
    pinMode(i, OUTPUT);
  }
  
  //Serial.begin(9600);
  lcd.begin();
  firstLine("Starting...");
  //setupKeyPad();
  delay(1000);


  if (!SD.begin(chipSelect)) {
    clrDisplay("SD card Error!");
    while (1);
  }
  secondLine("Welcome !!");
  delay(1000);
}

//Repeating part
void loop() {
  //char key_input = keypad.getKey();
  char key_input = keyInput();

  if (key_input) {
    mode = key_input;
  }

  if (mode == '1') {
    record();
    mode = '8';
  }

  if (mode == '2') {
    clrDisplay("Play Mode >");
    secondLine("File number: ");
    readFromFile();
    mode = '8';
  }

  if (mode == '*') {
    File root = SD.open("/");
    clrDisplay("Delete all files ? ");
    secondLine("Yes-1 No-2 ");

    if (getKeyInput() == "1.TXT") {
      deleteAll(root);
      root.close();
      clrDisplay("Files deleted !!");
      delay(1000);
    }
    else {
      clrDisplay("Leaving...");
      delay(1000);
    }
    root.close();

    mode = '8';

  }

  if (mode == '#') {
    File root = SD.open("/");
    clrDisplay("Tracks : ");
    getTrackList(root);
    root.close();
    mode = '8';
  }

  if (mode == '0') {
    File root = SD.open("/");
    clrDisplay("Delete mode X");
    secondLine("File to delete:");
    deleteFile(root);
    root.close();
    clrDisplay("Leaving...");
    delay(1000);
    mode = '8';
  }

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
