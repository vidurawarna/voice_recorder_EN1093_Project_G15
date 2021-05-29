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
#define no_of_Coffs 81
#define middle_Coff (no_of_Coffs-1)/2
#define pi 3.142

#define bufflen 25
#define filterlen 11

//for building cos wave
#define freqShift 500
#define w 2*pi*freqShift/sampleRate
#define sampleCounter sampleRate/freqShift

/*latest code takes 100microsec to read a single data - 12.5kHz
   takes 100microsec to read and output one data - 12.5kHz
   (Updated 2021/5/26 6.07pm)
*/
