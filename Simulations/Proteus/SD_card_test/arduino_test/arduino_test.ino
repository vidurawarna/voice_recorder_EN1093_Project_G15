#include "Headers.h"
#include "pin_Config.h"
int pot_Read;
char mode= '0';
int count = 0;
/* mode = 0 for record
 *  mode = 1 for display values
 *  mode = 2 for delete all files
 *  mode = 8 for pause state
  */
 
File test_File;

//setting up the keypad
 const byte r = 4;
  const byte c = 3;
  char keys[r][c]={
    {'1','2','3'},
    {'4','5','6'},
    {'7','8','9'},
    {'*','0','#'}
    };

    byte rPins[r] = {3,5,6,7};
    byte cPins[c] = {8,9,10};

    Keypad keypad = Keypad(makeKeymap(keys),rPins,cPins,r,c);
void setup() {
  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  Serial.print("Initializing SD card...");

  if (!SD.begin(4)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");



  setKeyPad();
}

void loop() {
  char k = keypad.getKey();

 if (k){mode = k;}
  if (mode == '0'){
    pot_Read = analogRead(pot);
    test_File.println(pot_Read);
    if (count == 0){
        // open the file.
        test_File = SD.open("test.txt", FILE_WRITE);
      
        // if the file opened okay, write to it:
        if (!test_File) {
          Serial.println("error opening file");
        }
      Serial.println("Recording");
    }else{
      Serial.print(".");
    }
    delay(1000);
    
    count ++;
    if (count==10){
      count = 0;
      mode = '8';
      Serial.println("\n Data recorded.");
      test_File.close();
    }
  }
  if (mode == '1'){
    Serial.println("\n\n Read Mode !!!");
    readFromFile("test.txt");
    mode= '8';
  }
   if (mode == '2'){
    SD.remove("test.txt");
    Serial.println("\n\nContent deleted from text file !!!");
    mode = '8';
  }
}
