
//>--------------------------------------< FREQUENCY SHIFTING >--------------------------------------<
void sig_freqShift() {
  //if (!SD.exists("SHIFT" + fname_temp)) {
  if (!SD.exists("S" + String(fname_temp[0]) + ".bin")) {
    //File out = SD.open("SHIFT" + fname_temp, FILE_WRITE);
    secondLine("Processing");
    File out = SD.open("S" + String(fname_temp[0]) + ".bin", FILE_WRITE);
    //makeWaveFile(out);
    File target = SD.open(fname_temp, FILE_READ);
    target.seek(44);

    byte buff[bufflen];
    int cosWave12_5[25] = {10, 10, 9, 7, 5, 3, 1, -2, -4, -6, -8, -9, -10, -10, -9, -8, -6, -4, -2, 1, 3, 5, 7, 9, 10};
    byte count = 0;
    byte buffCount = 0;

    while (target.available()) {

      buff[buffCount++] = (byte)((int)(target.read() - 127) * cosWave12_5[count++] / 10 + 127);
      if (count == 25 )
      {
        count = 0;
      }
      if (buffCount == bufflen) {
        buffCount = 0;
        out.write((byte*)buff, bufflen);
      }

    }

    //finalizeWave(out);
    out.close();
    target.close();
  }
  if (!SD.exists("SHIFT" + fname_temp)) {
    convolve();
  }
  fname_temp = "SHIFT" + fname_temp;
}
//END OF FREQUENCY SHIFTING

//CODE AFTER THIS IS NOT IMPLEMENTED FULLY YET

void convolve() {
  //Serial.println("con");
  //firstLine("con");
  int filter[filterlen] = {0, 1, 5, -4, -48, 920, -48, -4, 5, 1, 0};
  //int filter[filterlen] ={0,0,-13,18,270,480,270,18,-13,0,0};
  byte signal_in[filterlen];
  byte temp_buff[temp_buff_size];
  float temp = 0;
  byte temp_count = 0;


  File out = SD.open("SHIFT" + fname_temp, FILE_WRITE);
  //File out = SD.open("test.wav", FILE_WRITE);
  makeWaveFile(out);
  File target = SD.open("S" + String(fname_temp[0]) + ".bin", FILE_READ);
  //File target = SD.open("D.wav", FILE_READ);
  //target.seek(44);

  //unsigned long fSize = target.size() - 44;
  unsigned long fSize = target.size();

  target.read(signal_in, filterlen);
  target.read(temp_buff, temp_buff_size);
  //target.seek(45);

  while (fSize) {
    //t = micros();
    if (temp_count == temp_buff_size) {
      target.read(temp_buff, temp_buff_size);
      temp_count = 0;
    }
    temp = 127;
    //temp_ = 0;

    for (byte i = 0; i < filterlen - 1; i++) {
      temp += ((float(signal_in[i]) - 127) * filter[i] / 1000);//570
      signal_in[i] = signal_in[i + 1];
    }
    temp += ((float(signal_in[filterlen - 1]) - 127) * filter[filterlen - 1] / 1000);//570
    signal_in[filterlen - 1] = temp_buff[temp_count++];


    //temp_ = byte(temp + 127) ;
    out.write(byte(temp));
    //Serial.println(String(micros()-t));
    fSize --;


  }
  finalizeWave(out);
  out.close();
  target.close();
  //Serial.println("stop");
}
