
//###############################################
//LIBRARIES
//###############################################

#include <SPI.h>
#include <SD.h>
#include <Wire.h>

#include "src/LCDScreen/LCDScreen.h"

//###############################################
//PIN  CONFIGURATIONS
//###############################################
/*
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 10
*/

//pin map
#define mic A0
#define keypadPin A2
#define pot A3
//###############################################
//common values
//###############################################

//Audio file attributes
#define fsDelayin 100
#define fsDelayout 100
#define sampleRate 10000
#define byteRate (sampleRate/8)*monoStereo*8
#define monoStereo 1

//for filter coefficients
#define no_of_Coffs 81
#define middle_Coff (no_of_Coffs-1)/2
#define pi 3.142

//for building cos wave
#define freqShift 500
#define w 2*pi*freqShift/sampleRate
#define sampleCounter sampleRate/freqShift

/*latest code takes 100microsec to read a single data - 10kHz
   takes 100microsec to read and output one data - 10kHz
   (Updated 2021/5/2 2.36pm)
*/
