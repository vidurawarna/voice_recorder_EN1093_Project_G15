/*
   Definitions and implimentations all the functions used in recorder
*/
//>-----------------------------------< KEYPAD FUNCTIONS >-------------------------------------<

char keyInput() {
  /*
    This function detects a keypress and return the corrosponding key
  */
  char k = 0;
  if (analogRead(keypadPin) < 1023) {
    for (uint8_t i = 0; i < 8; i++) {
      if (abs(analogRead(keypadPin) - realVals[i]) < 5) {
        k = keys[i];
        while (analogRead(keypadPin) < 1000) {};
      }
    }
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
  checkDuplicates(65);
  File test_File = SD.open(fname_temp, FILE_WRITE);

  if (!test_File) {
    clrDisplay("error !!");
    delay(1000);
  }
  else {
    clrDisplay("Recording....");
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
      delayMicroseconds(18);
      // Serial.println(micros() - t);
    }
    finalizeWave(test_File);
    clrDisplay("Record saved.");
    delay(1000);
  }
  test_File.close();
}

void playTrack()
{
  /*This function reads data from the specified file and play*/

  checkChanges();//check for frequency change requirements

  File test_File = SD.open(fname_temp);

  if (!test_File) {
    // if the file didn't open, print an error:
    secondLine("No such track");
    delay(1000);
  }
  else {
    test_File.seek(44);
    clrDisplay("Playing Track ");
    secondLine(fname_temp);
    
    //Check whether a freaquency scale is set
    //>---------------------------< NORMAL OUTPUT >----------------------------------<
    
    if (freqScal == 0 || freqScal == 1) {
      while (test_File.available()) {
        //t = micros();
        char key = keyInput();
        if (key && key == 'p') {
          break;
        }
        PORTD = test_File.read();
        delayMicroseconds(50);
        //Serial.println(micros() - t);
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
          PORTD = test_File.read();//Accept the first sample among (# of samples=freqScal)
          delayMicroseconds(50);
          //Serial.println(micros() - t);
        } else {
          byte temp = test_File.read();//This is to neglet samples in between
        }

        count++;

        if (count == freqScal + 1) {//resetting the count
          count = 1;
        }
        //Serial.println(micros() - t);
      }
    }
    // close the file:
    secondLine("End of play");
    test_File.close();
    delay(1000);
  }
}

void checkChanges() {
  /*
   * This function checks for frequency change requirements
  */
  byte m = analogRead(pot) * (255. / 1023.);
  if (m < 90) {
    freqScal = 1;
  }
  else if (m < 180) {
    freqScal = 2;
  }
  else {
    freqScal = 3;
  }
}
//END OF RECORD AND PLAY FUNCTIONS

//>--------------------------------------< FILE HANDLING FUNCTIONS >--------------------------------------<

void getTrackList(byte count) {
  /*
   * This function checks for files and make a list of available files
   * Program only accept 15 tracks
   * counts the number of files
  */
  files = 0;
  fname_temp = String(char(count + 65)) + ".WAV";
  byte arrIndex = 0;
  while (true) {
    if (arrIndex == 15 || count == 26) {
      break;
    }
    if (SD.exists(fname_temp)) {
      tracks[arrIndex++] = fname_temp[0];
      files++;
    }
    fname_temp = String(char(++count + 65)) + ".WAV";

  }
  for (byte i = arrIndex; i < 15; i++) {
    tracks[i] = '_';
  }
}
byte nextTrack(byte count) {
  /*
     Checks tracks in order and returns the next track
  */
  count++;
  if (tracks[count] == '_') {
    secondLine("End of Tracks");
    delay(1000);
    secondLine("                ");
    delay(200);
    count--;
  }
  fname_temp = String(char(tracks[count])) + ".WAV";
  secondLine(fname_temp);
  //Serial.println(count);
  return count;
}
byte previousTrack(byte count) {
  /*
     Checks tracks in order and returns the previous track
  */
  if (count == 0) {
    secondLine("End of Tracks");
    delay(1000);
    secondLine("                ");
    delay(200);
    count++;
  }
  fname_temp = String(char(tracks[--count])) + ".WAV";
  secondLine(fname_temp);
  //Serial.println(count);
  return count;
}
void checkDuplicates(byte count) {

  /*This function checks if the new file to be made is existing,
    if does it generates a new name for the file*/

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
