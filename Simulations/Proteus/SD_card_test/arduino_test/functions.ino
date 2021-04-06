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

Keypad setKeyPad(){
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
}
