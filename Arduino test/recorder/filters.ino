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

    //float highPass[filterlen] = {0.00002549 , 0.00104805 , 0.00491863, -0.00418244, -0.04804388, 0.92006927, -0.04804388, -0.00418244, 0.00491863, 0.00104805, 0.00002549 };
    byte buff[bufflen];
    //int cosWave12_5[25] = {1, 1, 1, 1, 1, 0, 0, 0, 0, -1, -1, -1, -1, -1, -1, -1, -1, 0, 0, 0, 0, 1, 1, 1, 1};
    //float cosWave12_5[25] = {1.0,0.9686,0.8763,0.729,0.5358,0.309,0.0628,-0.1874,-0.4258,-0.6374,-0.809,-0.9298,-0.9921,-0.9921,-0.9298,-0.809,-0.6374,-0.4258,-0.1874,0.0628,0.309,0.5358,0.729,0.8763,0.9686};
    //int cosWave12_5[25] = {100,97,88,73,54,31,6,-19,-43,-64,-81,-93,-99,-99,-93,-81,-64,-43,-19,6,31,54,73,88,97};
    int cosWave12_5[25] = {10,10,9,7,5,3,1,-2,-4,-6,-8,-9,-10,-10,-9,-8,-6,-4,-2,1,3,5,7,9,10};
    byte count = 0;
    byte buffCount = 0;

    while (target.available()) {

      buff[buffCount++] = (byte)((int)(target.read() - 127) * cosWave12_5[count++]/10 + 127);
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
  secondLine("convolve");
  //long filter[filterlen] = {0.00002549 , 0.00104805 , 0.00491863, -0.00418244, -0.04804388, 0.92006927, -0.04804388, -0.00418244, 0.00491863, 0.00104805, 0.00002549 };
  int filter[filterlen] = {0, 0, 1, 0, -5, 92, -5, 0, 1, 0, 0};
  int temp[filterlen] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  int signal_out[bufflen + filterlen];
  int signal_in[bufflen];
  byte signal_out_byte[bufflen + filterlen];
  byte buffCount = 0;
  byte c = 0;
  secondLine("convolve");
  File out = SD.open("B.wav", FILE_WRITE);
  makeWaveFile(out);
  File target = SD.open("Z.wav", FILE_READ);
  target.seek(44);

  while (target.available()) {
    signal_in[buffCount++] = (int)(target.read()) - 127;
    if (buffCount == bufflen) {
      buffCount = 0;

      for (c = 0; c < (bufflen + filterlen); c++) {
        signal_out[c] = 0;
      }
      c = 0;

      for (byte i = 0; i < (bufflen + filterlen); i++)
      {
        for (byte j = 0; j < filterlen; j++)
        {
          //          if ((i + j) < filterlen) {
          //            signal_out[i + j] += (int)(signal_in[i] * filter[j]/100 + temp[c++]);
          //          }
          // else {
          signal_out[i + j] += (int)(signal_in[i] * filter[j] / 100);
          // }
        }
      }
      c = 0;
      for (byte i = 0; i < (bufflen ); i++)
      {
        //  if (i < bufflen) {
        signal_out_byte[i] = (byte)(signal_out[i] / 2  + 127);
        //  }
        //        else {
        //          temp[c++] = signal_out[i]/100;
        //        }
      }

      out.write((byte*)signal_out_byte, bufflen);

    }
  }
  finalizeWave(out);
  out.close();
  target.close();
  secondLine("finish");
}
