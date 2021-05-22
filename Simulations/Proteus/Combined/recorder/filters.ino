void filter(float cut_Off1, float cut_Off2, byte m) {
  /*
     m = 1 lowpass
     m = 2 highpass
     m = 3 bandpass
  */
  cut_Off1 = cut_Off1 / sampleRate;
  cut_Off2 = cut_Off2 / sampleRate;

  float H1[no_of_Coffs], H2[no_of_Coffs];
  float sum1 = 0, sum2 = 0;
  int i;

  for (i = 0; i < no_of_Coffs; i++) {

    if (i - no_of_Coffs / 2 == 0) {
      if (m == 1 || m == 3) {
        H1[i] = (float)(2 * pi * cut_Off1);
      }
      if (m == 2 || m == 3) {
        H2[i] = (float)(2 * pi * cut_Off2);
      }
    }

    else {
      if (m == 1 || m == 3) {
        H1[i] = (float)(sin(2 * pi * cut_Off1 * (i - no_of_Coffs / 2)) / (i - no_of_Coffs / 2));
      }
      if (m == 2 || m == 3) {
        H2[i] = (float)(sin(2 * pi * cut_Off2 * (i - no_of_Coffs / 2)) / (i - no_of_Coffs / 2));
      }
    }

    if (m == 1 || m == 3) {
      H1[i] = H1[i] * float((0.54 - 0.46 * cos(2 * pi * i / no_of_Coffs)));
    }
    if (m == 2 || m == 3)H2[i] = H2[i] * float((0.54 - 0.46 * cos(2 * pi * i / no_of_Coffs)));
  }

  for (i = 0; i < no_of_Coffs; i++) {
    if (m == 1 || m == 3) {
      sum1 += H1[i];
    }
    if (m == 2 || m == 3) {
      sum2 += H2[i];
    }
  }

  for (i = 0; i < no_of_Coffs; i++) {
    if (m == 1) {
      H1[i] = H1[i] / sum1;
    }
    if (m == 2) {
      H1[i] = -H2[i] / sum2;
    }
    if (m == 3) {
      H1[i] = (H2[i] / sum2) - (H1[i] / sum1);
    }
    Serial.println(H1[i]);
  }

  if (m == 2) {
    H1[middle_Coff] = H1[middle_Coff] + 1;
  }

}

void cosFunc() {

  byte t;

  File test_File = SD.open("cos500Hz.bin", FILE_WRITE);
  for (long i = 0; i < sampleCounter*30000; i++) {
    t = (byte)(127.5 + 127.5 * cos(w * i));
test_File.write(t);
  }
 
 
  test_File.close();
}
