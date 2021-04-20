void readFromFile(String filename)
{
  test_File = SD.open(filename);
  if (test_File) {
    Serial.println("Content : ");

    while (test_File.available()) {
      Serial.write(test_File.read());
    }
    // close the file:
    test_File.close();
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening file");
  }
}

int record(int count, int reading) {
  if (count == 0) {
    // open the file.
    File root = SD.open("/");
    file_name = String(countFiles(root) + 1) + ".txt";
    root.close();
    test_File = SD.open(file_name, FILE_WRITE);

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
    //Serial.println(entry.name());
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

void deleteFile(File r, int n ) {
  while (true) {
    File dir = r.openNextFile();
    if (!dir) {
      Serial.println("No such file.");
      break;
    } else if (String(dir.name()) == (String(n) + ".txt")) {
      SD.remove(dir.name());
      break;
    }
  }
}
