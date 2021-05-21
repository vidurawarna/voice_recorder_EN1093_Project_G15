/*
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

//############################################ LCD DISPLAY FUNCTIONS ####################################
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

//############################################ RECORD AND PLAY FUNCTIONS ####################################

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
      //   t = micros();

      pot_Read = analogRead(pot) * (255. / 1023.);

      char key = keyInput();

      if (key && key == 's') {
        break;
      }

      test_File.write(pot_Read);
      //delayMicroseconds(10);
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
    if (freqScal == 0 || freqScal == 1) {
      while (test_File.available()) {
        //t = micros();
        char key = keyInput();
        if (key && key == 's') {
          break;
        }
        PORTD = test_File.read();
        delayMicroseconds(35);
        //Serial.println(micros() - t);
      }
    }
    //Output for freaquency scaled track
    //Using down sampling
    else {
      byte count = 1;
      while (test_File.available()) {
        //t = micros();
        char key = keyInput();
        if (key && key == 's') {
          break;
        }

        if (count == 1) {
          PORTD = test_File.read();//Accept the first sample among (# of samples=freqScal)
          delayMicroseconds(35);
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
    clrDisplay("Track on Play:");
    delay(500);
    secondLine(fname_temp);
  }
}

void player() {
  /*
   * This is the player mode
   * It loads the tracks in alphebetic order
   * Press 'Play' when a track is loaded to the player
   * Press 'Stop' to stop playing
   * Press 'next' or 'previous' to toggle between tracks
   * Press 'Stop' in track loaded mode to exit player mode
  */
  clrDisplay("Track on Play:");
  delay(1000);
  if (countFiles() == 0) {
    clrDisplay("No Tracks !");
    delay(1000);
  }
  else {
    //Loads the first track in player
    byte fcount = nextTrack(64);

    while (true) {
      //Checks for key input
      char key = keyInput();
      if (key) {
        if (key == 'p') {
          //Play the track
          playTrack();
        }
        else if (key == '>') {
          //Load the next track
          fcount = nextTrack(fcount);
        }
        else if (key == '<') {
          //load the previous track
          fcount = previousTrack(fcount);
        }
        else if (key == 's') {
          //Exit from player mode
          break;
        }
        else if (key == 'd') {
          //This mode deletes the track loaded in payer
          clrDisplay("Delete Track?");
          secondLine("OK    No(MOD)");
          while (true) {
            char key = keyInput();
            if (key && key == 'o') {
              SD.remove(fname_temp);
              clrDisplay("Deleted !");
              delay(1000);
              break;
            } else if (key) {
              clrDisplay("Not Deleted !");
              delay(1000);
              break;
            }
          }
          if(countFiles()==0){break;}
          clrDisplay("Track on Play:");
          fcount = nextTrack(64);
        }
      }
    }
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
byte nextTrack(byte count) {
  /*
   * Checks tracks in alphetic order and returns the next track
  */
  fname_temp = String(char(++count)) + ".WAV";
  while (true) {
    if (SD.exists(fname_temp) || count >= 80) {
      if (count >= 80) {
        secondLine("End of Tracks");
        delay(1000);
        secondLine("                ");
        delay(500);
        count = nextTrack(64);
      }
      else {
        secondLine(fname_temp);
      }
      break;
    } else {
      fname_temp = String(char(++count)) + ".WAV";
    }
  }
  //Serial.println(count);
  return count;
}
byte previousTrack(byte count) {
  /*
   * Checks tracks in alphetic order and returns the previous track
  */
  fname_temp = String(char(--count)) + ".WAV";
  while (true) {
    if (SD.exists(fname_temp) || count <= 65) {
      if (count < 65) {
        secondLine("End of Tracks");
        delay(1000);
        secondLine("                ");
        delay(500);
        count = nextTrack(64);
      }
      else {
        secondLine(fname_temp);
      }
      break;
    } else {
      fname_temp = String(char(--count)) + ".WAV";
    }
  }
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

//################################### FUNCTIONS FOR WAVE FILE CREATION #######################################

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
