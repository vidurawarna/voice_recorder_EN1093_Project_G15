#include <LiquidCrystal.h>
#include <Keypad.h>

const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 3; //three columns

LiquidCrystal lcd(2,1,13,12,11,10);

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};

byte pin_rows[ROW_NUM] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {5, 4, 3}; //connect to the column pin

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

void setup() {
  lcd.begin(16,2);
  lcd.setCursor(5,0);
  lcd.print("Welcome");
  delay(1000);
  lcd.clear();
}

String playList[10]={"Track1","Track2","Track3","Track4","Track5","Track6","Track7","Track8","Track9","Track10"};
void loop(){
  lcd.print("Select one to play");
  delay(500);
  lcd.clear();
  for (int i =0; i<=9; i++){
    lcd.print(i+1);
    lcd.print(" ");
    lcd.print(playList[i]);
    delay(500);
    lcd.clear();
  }
  lcd.print("Select!");
  delay(500);
  lcd.clear();
  /*boolean NO_KEY = true;
  int key = keypad.getKey();
  while(key == NO_KEY){
    lcd.print("Select one");
    delay(1000);
  }
  if(key !=NO_KEY){
    lcd.print(key);
    delay(1000);
    lcd.clear();
    }
  }*/
  int key = keypad.getKey();
  /*lcd.print(key);
  delay(500);
  lcd.clear();*/
  while (key ==0){
    key = keypad.getKey();
  }
  if(key !=0){
    lcd.print(key-48);
    lcd.print(" ");
    lcd.print(playList[key-49]);
    lcd.print(" is playing");
    delay(1000);
    lcd.clear();
  }
  lcd.print("next");
  delay(500);
  lcd.clear();
}
