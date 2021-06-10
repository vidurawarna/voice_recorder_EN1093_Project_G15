//>--------------- LIBRARIES -------------------<

#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include "src/LCDScreen/LCDScreen.h"

//>----------- PIN  CONFIGURATIONS -------------<

/*
 SD card attached to SPI bus as follows:
   * MOSI - pin 11
   * MISO - pin 12
   * CLK - pin 13
   * CS - pin 10
*/

#define mic A0
#define keypadPin A2
#define ScalePOT A3
#define shiftEnhancePOT A1
#define speaker 9
#define sdcard 10

//>-------------- common values ----------------<

//Audio file attributes
#define sampleRate 12500
#define byteRate (sampleRate/8)*monoStereo*8
#define monoStereo 1
#define maxFiles 15
#define lcdAddr 0x27

//for filter coefficients
#define bufflen 25
#define filterlen 11
#define temp_buff_size 50

/*latest code takes 100microsec to read a single data - 12.5kHz
   takes 100microsec to read and output one data - 12.5kHz
   (Updated 2021/5/26 6.07pm)
*/
