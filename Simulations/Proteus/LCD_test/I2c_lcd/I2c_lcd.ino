#include <Wire.h>
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x20,16,2);
#include <Keypad.h>

const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 3; //three columns

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
  lcd.print("Play list");
  delay(1000);
  lcd.clear();
  play_list();
  // put your setup code here, to run once:
}

String playList[10]={"Audio 1","Audio 2","Audio 3","Audio 4","Audio 5","Audio 6","Audio 7","Audio 8","Audio 9","Audio 10"};

void play_list(){
  for (int i =0; i<=9; i++){
    lcd.setCursor(0,0);
    lcd.print("Track ");
    lcd.print(i+1);
    delay(10);
    lcd.setCursor(0,2);
    lcd.print(playList[i]);
    delay(1000);
    lcd.clear();
  }
} 
void play_audio(){
  int key = keypad.getKey();
  while (key ==0){
    key = keypad.getKey();
  }
  if(key !=0){
    //lcd.print(key-48);
    lcd.print(" ");
    lcd.print(playList[key-49]);
    lcd.print(" is playing");
    delay(3000);
    lcd.clear();
  }
}

void loop() {
  lcd.print("Select one ");
  lcd.setCursor(0,2);
  lcd.print("to play!");
  delay(1000);
  lcd.clear();
  play_audio();
  
  lcd.print("next");
  delay(1000);
  lcd.clear();
}
