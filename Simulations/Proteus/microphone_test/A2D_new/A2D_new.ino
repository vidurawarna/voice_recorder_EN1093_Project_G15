#include <EEPROM.h> 

// pin definitions
int ledPin = 13;
int buttonPin = 12;

// global variables
int lastButtonState = 1;
int debounceTime = 20;
int readVal;
int i=0;

void setup() {
  // setup pin modes
  pinMode(ledPin, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  for (int i=0;i<8;i++){
  pinMode(i,OUTPUT);
  }

  //initialise Serial port
  Serial.begin(9600);

  //assign counter the value of address 0
  readVal = EEPROM.read(0);
  //write a 0 to address 0. This allows for consecutive resets to reset the counter
  EEPROM.write(0,0);
}

void loop() {  
  int buttonState = digitalRead(buttonPin);   //read the state of buttonPin and store it as buttonState (0 or 1)

      while (buttonState == 0) {
      int waveValue = analogRead(A0);
      int waveVal = waveValue*(255./1023.);
      Serial.println(waveVal);
      EEPROM.write(i, waveVal); //write counter to address 0
      digitalWrite(ledPin, HIGH);//momentary LED
      delayMicroseconds(8);
      i = i+1;
      buttonState = digitalRead(buttonPin);
    }
 
for (int t=0;t<=i;t++){
  PORTD = EEPROM.read(t);
  Serial.println(EEPROM.read(t));
  delayMicroseconds(8);
   }  
}
