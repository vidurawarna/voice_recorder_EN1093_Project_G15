
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

#define pot A0
#define speaker A1
#define keypadPin A2
#define fsDelayin 100
#define fsDelayout 100
#define sampleRate 10000
#define byteRate (sampleRate/8)*monoStereo*8
#define monoStereo 1
/*latest code takes 100microsec to read a single data - 10kHz
 * takes 100microsec to read and output one data - 10kHz
 * (Updated 2021/5/2 2.36pm)
*/
