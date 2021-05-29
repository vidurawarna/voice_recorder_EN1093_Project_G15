/*
   Definitions and implimentations all the functions used in recorder
*/
//>-----------------------------------< KEYPAD FUNCTIONS >-------------------------------------<

char keyInput() {
  /*
    This function detects a keypress and return the corrosponding key
  */
  char k = 0;
  //  if (analogRead(keypadPin) < 1000) {
  //    for (uint8_t i = 0; i < 8; i++) {
  //      if (abs(analogRead(keypadPin) - realVals[i]) < 5) {
  //        k = keys[i];
  //        while (analogRead(keypadPin) < 1000) {};
  //      }
  //    }
  //  }
  if (byte m = ~PIND) {

    switch (m) {
      case 1: k = 's'; break;
      case 2: k = '<'; break;
      case 4: k = 'p'; break;
      case 8: k = '>'; break;
      case 32: k = 'd'; break;
      case 64: k = 'm'; break;
      case 128: k = 'o'; break;
      default: k = 0; break;
    }
    delay(300);
    PORTD = 0b11111111;

  }
  return k;
}

//END OF KEYPAD FUNCTIONS

//>--------------------------------< LCD DISPLAY FUNCTIONS >-----------------------------------<
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
//END OF LCD DISPLAY FUNCTIONS

//>-----------------------------< RECORD AND PLAY FUNCTIONS >----------------------------------<

void record() {
  /*Used to record the data got from input into a file*/
  checkDuplicates();
  File test_File = SD.open(fname_temp, FILE_WRITE);

  if (!test_File) {
    clrDisplay("Error");
    delay(1000);
  }
  else {
    clrDisplay("Recording");
    makeWaveFile(test_File);
    byte pot_Read;

    while (true) {
      //t = micros();

      pot_Read = analogRead(mic) * (255. / 1023.);

      char key = keyInput();

      if (key && key == 's') {
        break;
      }

      test_File.write(pot_Read);
      delayMicroseconds(16);
//            t = micros() - t;
//            clrDisplay(String(t));
//            delay(500);
    }
    finalizeWave(test_File);
    clrDisplay("Saved");
    delay(1000);
  }
  test_File.close();
}

void playTrack()
{
  /*This function reads data from the specified file and play*/

  checkChanges();//check for frequency change requirements

  if(shift)
  {
    secondLine("Processing");
    sig_freqShift();
  }

  File test_File = SD.open(fname_temp);

  if (!test_File) {
    // if the file didn't open, print an error:
    secondLine("Error");
    delay(1000);
  }
  else {
    test_File.seek(44);
    clrDisplay("Now Playing");
    secondLine(fname_temp);

    //Check whether a freaquency scale is set
    //>---------------------------< NORMAL OUTPUT >----------------------------------<

    if (freqScal == 0 || freqScal == 1) {
      while (test_File.available()) {
        //t = micros();
        analogWrite(speaker, int(test_File.read()));
        delayMicroseconds(40);
        char key = keyInput();
        if (key && key == 'p') {
          break;
        }
        
        //Serial.println(micros() - t);
//        t = micros() - t;
//        clrDisplay(String(t));
//        delay(1000);
      }
    }

    //>---------------------------< SCALED OUTPUT >----------------------------------<
    //Output for freaquency scaled track
    //Using down sampling

    else {
      byte count = 1;
      while (test_File.available()) {
        //t = micros();
        char key = keyInput();
        if (key && key == 'p') {
          break;
        }

        if (count == 1) {
          analogWrite(speaker, test_File.read()); //Accept the first sample among (# of samples=freqScal)
          delayMicroseconds(40);
          //Serial.println(micros() - t);
//          t = micros() - t;
//          clrDisplay(String(t));
//          delay(1000);
        } else {
          byte temp = test_File.read();//This is to neglet samples in between
        }

        count++;

        if (count == freqScal + 1) {//resetting the count
          count = 1;
        }
        //Serial.println(micros() - t);
        //        t = micros() - t;
        //        clrDisplay(String(t));
        //        delay(1000);
      }
    }
    // close the file:
    analogWrite(speaker, 0);
    secondLine("End of play");
    test_File.close();
    
    if(shift || enhance){
      fname_temp = String(char(tracks[fcount])) + ".WAV";
      shift = false;
      enhance = false;
    }
    
    delay(1000);
  }
}

void checkChanges() {
  /*
     This function checks for frequency change requirements
  */
    byte fsc = analogRead(ScalePOT) * (255. / 1023.);
    byte fshift = analogRead(shiftEnhancePOT) * (255. / 1023.);
    
    if (fsc < 90) {
      freqScal = 1;
    }
    else if (fsc < 180) {
      freqScal = 2;
    }
    else {
      freqScal = 3;
    }

    if(fshift < 90){
      shift = false;
      enhance = false;
    }
    else if(fshift < 180){
      shift = true;
      enhance = false;
    }
    else{
      shift = false;
      enhance = true;
    }
}
//END OF RECORD AND PLAY FUNCTIONS

//>--------------------------------------< FILE HANDLING FUNCTIONS >--------------------------------------<

void getTrackList() {
  /*
     This function checks for files and make a list of available files
     Program only accept 15 tracks
     counts the number of files
  */
  byte ASCIIcount = 65;
  files = 0;
  fname_temp = String(char(ASCIIcount)) + ".WAV";
  byte arrIndex = 0;
  while (true) {
    if (arrIndex == maxFiles || ASCIIcount == 91) {
      break;
    }
    if (SD.exists(fname_temp)) {
      tracks[arrIndex++] = fname_temp[0];
      files++;
    }
    fname_temp = String(char(++ASCIIcount)) + ".WAV";

  }
  for (byte i = arrIndex; i < maxFiles; i++) {
    tracks[i] = '_';
  }
}

void nextTrack() {
  /*
     Checks tracks in order and returns the next track
  */
  fcount++;
  if (tracks[fcount] == '_') {
    fcount = 0;
  }
  fname_temp = String(char(tracks[fcount])) + ".WAV";
  secondLine(fname_temp);
  //Serial.println(count);
}

void previousTrack() {
  /*
     Checks tracks in order and returns the previous track
  */
  if (fcount == 0) {
    fcount = files - 1;
  }
  else
  {
    fcount--;
  }
  fname_temp = String(char(tracks[fcount])) + ".WAV";
  secondLine(fname_temp);
  //Serial.println(count);
}

void checkDuplicates() {

  /*This function checks if the new file to be made is existing,
    if does it generates a new name for the file*/
  byte count = 65;
  fname_temp = String(char(count)) + ".wav";
  while (true) {
    if (SD.exists(fname_temp)) {
      fname_temp = String(char(++count)) + ".wav";
    } else {
      break;
    }
  }
}

//END OF FILE HANDLING FUNCTIONS

//>------------------------------< FUNCTIONS FOR WAVE FILE CREATION >---------------------------------<

void makeWaveFile(File sFile) {
  /*
     This function creates the wave header file required
     All bytes should be in little endian format, except String values
  */

  sFile.write((byte*)"RIFF    WAVEfmt ", 16);//Starting bytes of the wave header file
  byte chunk[] = {16, 0, 0, 0, 1, 0, 1, 0, lowByte(sampleRate), highByte(sampleRate)};
  /*
     chunk[]
     first 4 bytes: size of  previous data chunck
     next 2 bytes: Audio format (1 - PCM)
     next 2 byte: No of channels (Mono = 1, Stereo = 2) (in our case 1)
     last two are the first two bytes of sample rate
  */
  sFile.write((byte*)chunk, 10);

  chunk[0] = 0; chunk[1] = 0; //end of sample rate bytes

  //byteRate = (sampleRate/8)*monoStereo*8;
  chunk[2] = lowByte(byteRate); chunk[3] = highByte(byteRate); chunk[4] = 0; chunk[5] = 0; // byteRate

  //byte blockAlign = monoStereo * (bps/8);
  //this is always equal to 1 in 8bit PCM mono channel
  chunk[6] = 1; chunk[7] = 0; //BlockAlign

  chunk[8] = 8; chunk[9] = 0; //bits per sample

  sFile.write((byte*)chunk, 10);
  sFile.write((byte*)"data    ", 8);

}

void finalizeWave(File sFile) {
  /*
     This function finalizes the wave file
  */
  unsigned long fSize = sFile.size();

  fSize -= 8;
  sFile.seek(4);
  byte chunk2[4] = {lowByte(fSize), highByte(fSize), fSize >> 16, fSize >> 24};
  sFile.write(chunk2, 4);//Writing chunksize to 5 - 8 bytes in wave file

  sFile.seek(40);
  fSize -= 36 ;
  chunk2[0] = lowByte(fSize); chunk2[1] = highByte(fSize); chunk2[2] = fSize >> 16; chunk2[3] = fSize >> 24;
  sFile.write((byte*)chunk2, 4);//Writting num of samples to 41-44 bytes in wave file
}
//END OF WAVE FILE CREATE FUNCTIONS

//>------------------------------< FUNCTIONS FOR PWM SPEED CHANGE >---------------------------------<
void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if (pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch (divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if (pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if (pin == 3 || pin == 11) {
    switch (divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x07; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}
//END OF PWM SPEED CHANGE FUNCTION
