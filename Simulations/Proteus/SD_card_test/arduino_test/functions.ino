void readFromFile()
{
  test_File = SD.open(getKeyInput());
  if (test_File) {
    Serial.println("Content : ");

    while (test_File.available()) {
      Serial.write(test_File.read());
    }
    // close the file:
    Serial.println("End of play :)");
    test_File.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("No such file");
  }
}


int record(int count, int reading) {
  if (count == 0) {
    // open the file.
    File root = SD.open("/");
    int fcount = countFiles(root);    
    root.close();
    test_File = SD.open(checkDuplicates(fcount), FILE_WRITE);

    // if the file opened okay, write to it:
    if (!test_File) {
      Serial.println("error opening file");
    }
    Serial.println("Recording");
  } else {
    Serial.print(".");
  }

  test_File.println(reading);
  count ++;

  delay(1000);

  if (count == 10) {
    count = 0;
    mode = '8';
    Serial.println("Data recorded.");
    test_File.close();
  }
  return count;
}


int countFiles(File r) {
  int c = 0;
  while (true) {
    File dir = r.openNextFile();
    if (!dir) {
      break;
    }
    c++;
    //Serial.println(dir.name());
    dir.close();
  }
  return c;
}



void deleteAll(File r) {
  while (true) {
    File dir = r.openNextFile();
    if (!dir) {
      break;
    }
    SD.remove(dir.name());
  }
 
}


void deleteFile(File r) {
  String n = getKeyInput();
  while (true) {
    File dir = r.openNextFile();
    if (!dir) {
      Serial.println("No such file.");
      break;
    } else if (String(dir.name()) == n) {
      SD.remove(dir.name());
      Serial.println("File removed");
      break;
    }
  }
}

String getKeyInput() {
  String res = ""; 
  while (true) {
    char key_input = keypad.getKey();
    if (key_input == '*') {
      break;
    }
    else if (key_input) {
      Serial.print(key_input);
      res += String(key_input);

    }
  }
  Serial.println("");
  return res + ".TXT";
}

void getTrackList(File r){
  while (true) {
    File dir = r.openNextFile();
    if (!dir) {
      break;
    }
    
    Serial.println(dir.name());
    dir.close();
  }
}

String checkDuplicates(int count){
  String fname_temp =String(count + 1) + ".txt";
  if (SD.exists(fname_temp)){
      fname_temp = checkDuplicates(count+1);
    }
    else{
      return fname_temp;
    }
}
