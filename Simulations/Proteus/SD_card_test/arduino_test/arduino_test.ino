#include "Headers.h"
#include "pin_Config.h"

int pot_Read;

int count = 0;

char mode = '8';
/* mode = 1 for record
    mode = 2 for display values
    mode = 3 for delete all files
    mode = 8 for pause state
*/

File test_File;
String file_name = "test.txt";

//setting up the keypad
const byte r = 4;
const byte c = 3;
char keys[r][c] = {
  {'1', '2', '3'},
  {'4', '5', '6'},
  {'7', '8', '9'},
  {'*', '0', '#'}
};

byte rPins[r] = {3, 5, 6, 7};
byte cPins[c] = {8, 9, 10};

Keypad keypad = Keypad(makeKeymap(keys), rPins, cPins, r, c);

//Initializing things
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("Initializing...");

  if (!SD.begin(4)) {
    Serial.println("SD card Error!");
    while (1);
  }
  Serial.println("initialization done.");
}

//Repeating part
void loop() {
  char key_input = keypad.getKey();

  if (key_input) {
    mode = key_input;
  }

  if (mode == '1') {

    pot_Read = analogRead(pot);
    count = record(count, pot_Read);
  }

  if (mode == '2') {
    Serial.println("Play Mode !!!");
    Serial.print("Enter file number: ");
    readFromFile();
    mode = '8';
  }

  if (mode == '*') {
    File root = SD.open("/");
    Serial.println("Want to delete all files ? ");
    Serial.println("\tYes - 1");
    Serial.println("\tNo - 2");
    Serial.print("Choice: ");

    if (getKeyInput() == "1.TXT") {
      deleteAll(root);
      root.close();
      Serial.println("All files deleted !!!");
    }
    Serial.println("Leaving delete mode...");
    mode = '8';

  }

  if (mode == '#') {
    File root = SD.open("/");
    Serial.println("files : ");
    getTrackList(root);
    root.close();
    mode = '8';
  }

  if (mode == '0') {
    File root = SD.open("/");
    Serial.println("Delete mode !!!");
    Serial.print("Enter file number to delete: ");
    deleteFile(root);
    root.close();
    Serial.println("Leaving delete mode...");
    mode = '8';
  }
}
