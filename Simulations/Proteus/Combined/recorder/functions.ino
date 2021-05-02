/*
   WROTE BY : VIDURA MUNASINGHE
   https://github.com/vidurawarna
   Definitions and implimentations all the functions used in recorder
*/
//#################################### KEYPAD FUNCTIONS ######################################

char keyInput() {
  /*
    This function detects a keypress and return the corrosponding key
  */
  char k = 0;
  //  int PinVal = analogRead(keypadPin);
  //  Serial.println(PinVal);
  if (analogRead(keypadPin) < 1023) {
    for (char i = 0; i < 16; i++) {
      if (abs(analogRead(keypadPin) - realVals[i]) < 5) {
        k = keys[i];
        while (analogRead(keypadPin) < 1000);
      }
    }
  }
  return k;
}

String getKeyInput() {
  /*
     used to get the user inputs from the keypad
  */
  String res = "";
  while (true) {
    //char key_input = keypad.getKey();
    char key_input = keyInput();
    if (key_input == '*') {
      break;
    }
    else if (key_input) {
      res += String(key_input);
      clrDisplay(res);
    }
  }
  return res + ".BIN";
}
//END OF KEYPAD FUNCTIONS

//################################# RECORD AND PLAY FUNCTIONS ###########################################

void record() {
  /*Used to record the data got from input into a file*/

  int fcount = countFiles();

  String fi = checkDuplicates(fcount);//this function is used to check duplicates when new files are created

  File test_File = SD.open(fi, FILE_WRITE);
  if (!test_File) {
    clrDisplay("error !!");
    delay(1000);
  }
  else {
    clrDisplay("Recording....");
    byte pot_Read;

    while (true) {
      //t = micros();

      pot_Read = analogRead(pot) * (255. / 1023.);

      char key = keyInput();

      if (key && key == '*') {
        break;
      }
      if (keyInput()) {
        break;
      }

      test_File.write(pot_Read);
      //Serial.println(micros() - t);
    }
    clrDisplay("Data recorded.");
    delay(1000);
  }
  test_File.close();
}

void playTrack()
{
  /*This function reads data from the specified file and play*/

  clrDisplay("Play Mode >");
  secondLine("File number: ");

  String f = getKeyInput();

  File test_File = SD.open(f);

  if (test_File) {

    clrDisplay("Playing Track ");
    while (test_File.available()) {
      //t = micros();
      char key = keyInput();
      if (key && key == '*') {
        break;
      }

      PORTD = test_File.read();
      delayMicroseconds(50);
      //Serial.println(micros() - t);
    }

    // close the file:
    secondLine("End of play");
    test_File.close();
    delay(1000);

  } else {
    // if the file didn't open, print an error:
    secondLine("No such file");
    delay(1000);
  }
}
//END OF RECORD AND PLAY FUNCTIONS

//##################################### FILE HANDLING FUNCTIONS ##############################################

/*From here the functions countFiles, deleteAll,getTrackList and
  deleteFile use the same itteration method to handle file operations*/

int countFiles() {
  /*This loop checks for next file to open and
    when there are no fles it stops counting, then return the count*/
  File r = SD.open("/");
  int c = 0;
  while (true) {
    //File dir = r.openNextFile();
    if (!r.openNextFile()) {
      //dir.close();
      break;
    }
    c++;
    //dir.close();
  }
  r.close();
  return c;
}

String checkDuplicates(int count) {

  /*This function checks if the new file to be made is existing,
    if does it generates a new name for the file*/

  String fname_temp = String(count + 1) + ".bin";
  if (SD.exists(fname_temp)) {
    fname_temp = checkDuplicates(count + 1);
  }
  else {

    return fname_temp;
  }
}

void firstLine(String msg) {
  //Prints the string passed in the first line of the LCD display
  lcd.setCursor(0, 0);
  lcd.print(msg);
}

void clrDisplay(String msg) {
  //Clears the LCD and displays the msg in first line
  lcd.clear();
  firstLine(msg);
}

void secondLine(String msg) {
  //Prints the string passed in the second line of the LCD display
  lcd.setCursor(0, 1);
  lcd.print(msg);
}


void operation(int m) {
  /*
   * m = 1 for delete all
   * m = 2 for delete a single file
   * m = 3 for display files 
  */
  
  File r = SD.open("/");

  if (m == 1) {
    //********************** delete all **********************************
    while (true) {
      File dir = r.openNextFile();
      if (!dir) {
        dir.close();
        break;
      }
      SD.remove(dir.name());
      dir.close();
    }

  } else if (m == 2) {
    //********************** delete file *********************************
    clrDisplay("Delete mode X");
    secondLine("File to delete:");
    String n = getKeyInput();
    while (true) {
      File dir = r.openNextFile();
      if (!dir) {
        dir.close();
        clrDisplay("No such file.");
        delay(1000);
        break;
      } else if (String(dir.name()) == n) {
        SD.remove(dir.name());
        dir.close();
        clrDisplay("File removed");
        delay(1000);
        break;
      }
      dir.close();
    }

  } else {
    //********************** get files ***********************************
    clrDisplay("Tracks : ");
    while (true) {
      File dir = r.openNextFile();
      if (!dir) {
        dir.close();
        clrDisplay("End of files !!");
        delay(1000);
        break;
      }

      secondLine("Track " + String(dir.name()[0]));
      delay(1000);
      dir.close();
    }

  }
  r.close();
}
//END OF FILE HANDLING FUNCTIONS
