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
#define fs 8000
# define r1 3
# define r2 5
# define r3 6
# define r4 7
# define c1 8
# define c2 9
# define c3 10
