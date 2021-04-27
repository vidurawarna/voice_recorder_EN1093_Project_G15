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
  The circuit:
   SD card attached to SPI bus as follows:
 ** MOSI - pin 11
 ** MISO - pin 12
 ** CLK - pin 13
 ** CS - pin 4 (for MKRZero SD: SDCARD_SS_PIN)
*/

#define pot A0
#define speaker A1
#define keypadPin A3
#define fs 2000
#define fsDelay 125 //microseconds
#define buttonPin A2
/*latest code takes 428microsec to read a single data
 * takes 450microsec to read and output one data
 * (Updated 2021/4/26)
*/
# define r1 3
# define r2 5
# define r3 6
# define r4 7
# define c1 8
# define c2 9
# define c3 10
