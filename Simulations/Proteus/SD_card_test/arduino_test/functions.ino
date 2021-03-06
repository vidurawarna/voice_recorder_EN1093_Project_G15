void readFromFile()
{
  /*This function reads data from the specified file and display*/
  
  test_File = SD.open(getKeyInput());
  if (test_File) {
    clrDisplay("Content : ");

    while (test_File.available()) {
      secondLine(String(test_File.read()));
      delay(100);
    }
    // close the file:
    secondLine("End of play");
    delay(1000);
    test_File.close();
  } else {
    // if the file didn't open, print an error:
    secondLine("No such file");
    delay(1000);
  }
}


int record(int count, int reading) {
  /*Used to record the data got from input into a file*/
  if (count == 0) {
    // open the file.
    File root = SD.open("/");
    int fcount = countFiles(root);    
    root.close();
    test_File2 = SD.open(checkDuplicates(fcount), FILE_WRITE);//this function is used to check duplicates when new files are created

    // if the file opened okay, write to it:
    if (!test_File2) {
      clrDisplay("error !!");
      delay(1000);
    }
    
  } else {
    clrDisplay("Recording....");  
  }

  test_File2.println(reading);
  count ++;

  delay(1000);

  if (count == 10) {
    count = 0;
    mode = '8';
    clrDisplay("Data recorded.");
    delay(1000);
    test_File2.close();
  }
  return count;
}

/*From here the functions countFiles, deleteAll,getTrackList and 
deleteFile use the same itteration method to handle file operations*/

int countFiles(File r) {
  /*This loop checks for next file to open and 
  when there are no fles it stops counting, then return the count*/
  int c = 0;
  while (true) {
    File dir = r.openNextFile();
    if (!dir) {
      dir.close();
      break;
    }
    c++;
    dir.close();
  }
  return c;
}



void deleteAll(File r) {
  while (true) {
    File dir = r.openNextFile();
    if (!dir) {
      dir.close();
      break;
    }
    SD.remove(dir.name());
    dir.close();
  }
}


void deleteFile(File r) {
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
}

String getKeyInput() {
  //used to get the user inputs from the keypad
  String res = ""; 
  while (true) {
    char key_input = keypad.getKey();
    if (key_input == '*') {
      break;
    }
    else if (key_input) {
      clrDisplay(String(key_input));
      res += String(key_input);
    }
  }
  return res + ".TXT";
}

void getTrackList(File r){
  while (true) {
    File dir = r.openNextFile();
    if (!dir) {
      dir.close();
      clrDisplay("End of files !!");
      delay(1000);
      break;
    }
    
    secondLine(dir.name());
    delay(1000);
    dir.close();
  }
}

/*This function checks if the new file to be made is existing, 
if does it generates a new name for the file*/

String checkDuplicates(int count){
  String fname_temp =String(count + 1) + ".txt";
  if (SD.exists(fname_temp)){
      fname_temp = checkDuplicates(count+1);
    }
    else{
      return fname_temp;
    }
}

void firstLine(String msg){
  lcd.setCursor(0,0);
  lcd.print(msg);
}

void clrDisplay(String msg){
  lcd.clear();
  firstLine(msg);
}

void secondLine(String msg){
  lcd.setCursor(0,1);
  lcd.print(msg);
}
