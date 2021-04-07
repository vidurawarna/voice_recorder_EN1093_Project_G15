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

int record(int count,int reading){
  if (count == 0) {
      // open the file.
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
      Serial.println("\n Data recorded.");
      test_File.close();
    }

    return count;
}
