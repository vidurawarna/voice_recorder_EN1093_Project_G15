//void filter(float cut_Off1, float cut_Off2, byte m) {
//  /*
//     m = 1 lowpass
//     m = 2 highpass
//     m = 3 bandpass
//  */
//  cut_Off1 = cut_Off1 / sampleRate;
//  cut_Off2 = cut_Off2 / sampleRate;
//
//  float H1[no_of_Coffs], H2[no_of_Coffs];
//  float sum1 = 0, sum2 = 0;
//  int i;
//
//  for (i = 0; i < no_of_Coffs; i++) {
//
//    if (i - no_of_Coffs / 2 == 0) {
//      if (m == 1 || m == 3) {
//        H1[i] = (float)(2 * pi * cut_Off1);
//      }
//      if (m == 2 || m == 3) {
//        H2[i] = (float)(2 * pi * cut_Off2);
//      }
//    }
//
//    else {
//      if (m == 1 || m == 3) {
//        H1[i] = (float)(sin(2 * pi * cut_Off1 * (i - no_of_Coffs / 2)) / (i - no_of_Coffs / 2));
//      }
//      if (m == 2 || m == 3) {
//        H2[i] = (float)(sin(2 * pi * cut_Off2 * (i - no_of_Coffs / 2)) / (i - no_of_Coffs / 2));
//      }
//    }
//
//    if (m == 1 || m == 3) {
//      H1[i] = H1[i] * float((0.54 - 0.46 * cos(2 * pi * i / no_of_Coffs)));
//    }
//    if (m == 2 || m == 3)H2[i] = H2[i] * float((0.54 - 0.46 * cos(2 * pi * i / no_of_Coffs)));
//  }
//
//  for (i = 0; i < no_of_Coffs; i++) {
//    if (m == 1 || m == 3) {
//      sum1 += H1[i];
//    }
//    if (m == 2 || m == 3) {
//      sum2 += H2[i];
//    }
//  }
//
//  for (i = 0; i < no_of_Coffs; i++) {
//    if (m == 1) {
//      H1[i] = H1[i] / sum1;
//    }
//    if (m == 2) {
//      H1[i] = -H2[i] / sum2;
//    }
//    if (m == 3) {
//      H1[i] = (H2[i] / sum2) - (H1[i] / sum1);
//    }
//    Serial.println(H1[i]);
//  }
//
//  if (m == 2) {
//    H1[middle_Coff] = H1[middle_Coff] + 1;
//  }
//
//}


//>--------------------------------------< FREQUENCY SHIFTING >--------------------------------------<
void sig_freqShift() {
  if (!SD.exists("SHIFT" + fname_temp)) {

    File out = SD.open("SHIFT" + fname_temp, FILE_WRITE);
    makeWaveFile(out);
    File target = SD.open(fname_temp, FILE_READ);
    target.seek(44);

    byte buff[bufflen];
    int cosWave12_5[25] = {10, 10, 9, 7, 5, 3, 1, -2, -4, -6, -8, -9, -10, -10, -9, -8, -6, -4, -2, 1, 3, 5, 7, 9, 10};
    byte count = 0;
    byte buffCount = 0;

    while (target.available()) {

      buff[buffCount++] = (byte)((int)(target.read() - 127) * cosWave12_5[count++] / 5 + 127);
      if (count == 25 )
      {
        count = 0;
      }
      if (buffCount == bufflen) {
        buffCount = 0;
        out.write((byte*)buff, bufflen);
      }

    }

    finalizeWave(out);
    out.close();
    target.close();
  }
  fname_temp = "SHIFT" + fname_temp;
}
//END OF FREQUENCY SHIFTING

//CODE AFTER THIS IS NOT IMPLEMENTED FULLY YET

void convolve() {
  Serial.println("con");
  //int filter[26]={0,0,0,1,1,2,3,4,5,5,5,4,1,-2,-7,-13,-21,-30,-40,-50,-60,-68,-76,-82,-85,1000};
  //int filter[51] = {0,0,0,1,1,2,3,4,5,5,5,4,1,-2,-7,-13,-21,-30,-40,-50,-60,-68,-76,-82,-85,1000,-85,-82,-76,-68,-60,-50,-40,-30,-21,-13,-7,-2,1,4,5,5,5,4,3,2,1,1,0,0,0};
  int filter[51] = {0, 0, 0, 1, 1, 2, 3, 4, 4, 5, 4, 3, 1, -1, -6, -12, -20, -28, -37, -46, -55, -63, -70, -75, -78, 919, -78, -75, -70, -63, -55, -46, -37, -28, -20, -12, -6, -1, 1, 3, 4, 5, 4, 4, 3, 2, 1, 1, 0, 0, 0};
  //int filter[11] = {0,1,5,-5,-52,1000,-52,-5,5,1,0};
  // int temp[filterlen] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  //int signal_out[bufflen];
  //int filter[26];
  byte signal_in[bufflen];
  // byte tt[4]={1,1,1,1};
  byte signal_out[bufflen];
  //byte buffCount = 0;
  //byte c = 0;
  bool ok = true;
  //int temp;

  //  File coffs = SD.open("FILT.txt", FILE_READ);
  //  while(coffs.available()){
  //    filter[c++] = coffs.readStringUntil("\n").toInt();
  //  }
  //  coffs.close();
  //byte k[20] = {1,2,3,4,5,6,7,8,9,10,12,13,14,15,16,17,18,19,20};
  File out = SD.open("fC.wav", FILE_WRITE);
  makeWaveFile(out);
  File target = SD.open("SHIFTC.WAV", FILE_READ);
  target.seek(44);
  unsigned long fSize = target.size() - 44;
  unsigned long pos = target.position();

  target.read(signal_in, bufflen);
  while (pos != fSize) {
    int temp = 0;
    byte temp_;
    

    for (byte i = 0; i < bufflen; i++) {
     // if (signal_in[i] != 127 || filter[i] != 0) {
        temp += (int(signal_in[i]) - 127) * filter[i] / 1200;
      //}
    }
    //    char key = keyInput();
    //    if(key=='p'){break;}
    temp_ = byte(temp + 127) ;
    out.write(temp_);

    for (byte i = 0; i < bufflen; i++) {
        
        if(i == bufflen-1){signal_in[i] = target.read();}
        else{signal_in[i] = signal_in[i+1];}
     
    }
    
    //fSize -= 1;
    pos++;
    //target.seek(++pos);
  }
  finalizeWave(out);
  out.close();
  target.close();
  Serial.println("stop");
}


//  while (target.available()) {
//
//    signal_in[buffCount++] = (int)((target.read()) - 127);
//    //Serial.println((target.read()));
//    if (buffCount == bufflen) {
//      buffCount = 0;
//
//      for (c = 0; c < (bufflen); c++) {
//        signal_out_byte[c] = 127;
//      }
// c = 0;
//###############################################################################################
//      for (byte i = 0; i < (bufflen + filterlen); i++)
//      {
//        for (byte j = 0; j < filterlen; j++)
//        {
//                    byte jj = j;
//          if(j>23){jj = 44 - j;}
//         // if ((i + j) < filterlen) {
//          //  signal_out[i + j] += (int)(signal_in[i] * filter[j] / 1000 + temp[i + j]);
//         // }
//         // else {
//            signal_out_byte[i + j] += (int)(signal_in[i] * filter[jj] / 1000);
//         // }
//        }
//      }
//      c = 0;
//      for (byte i = 0; i < (bufflen ); i++)
//      {
//
//        if (i < bufflen) {
//          signal_out_byte[i] = (byte)(signal_out[i]/18  + 127);
//          if(signal_out_byte[i]>maxm){maxm = signal_out_byte[i];}
//        if(signal_out_byte[i]<minm){minm = signal_out_byte[i];}
//        }
//        else {
//          temp[c++] = signal_out[i];
//        }
//      }

//#########################################################################################3
//      for (byte i = filterlen; i < bufflen; i++) {
//        //signal_out[i] = 0;
//        for (byte j = 0; j < filterlen; j++) {
//          byte jj = j;
//          if(j>23){jj = 44 - j;}
//          signal_out_byte[i] += (int)(signal_in[i - j] * (filter[jj] / 1000));
//        }
//
//      }
//###########################################################################################

//      out.write((byte*)signal_out_byte, bufflen);
//
//    }
//  }

//Serial.println(maxm);
//  Serial.println(maxm);
//      Serial.println(minm);



//      for (byte i = 0; i < (bufflen + filterlen); i++)
//      {
//        for (byte j = 0; j < filterlen; j++)
//        {
//          if ((i + j) < filterlen) {
//            signal_out[i + j] += (int)(signal_in[i] * filter[j] / 1000 + temp[i + j]);
//          }
//          else {
//            signal_out[i + j] += (int)(signal_in[i] * filter[j] / 1000);
//          }
//        }
//      }
//      c = 0;
//      for (byte i = 0; i < (bufflen ); i++)
//      {
//
//        if (i < bufflen) {
//          signal_out_byte[i] = (byte)(signal_out[i]/18  + 127);
//          if(signal_out_byte[i]>maxm){maxm = signal_out_byte[i];}
//        if(signal_out_byte[i]<minm){minm = signal_out_byte[i];}
//        }
//        else {
//          temp[c++] = signal_out[i];
//        }
//      }
