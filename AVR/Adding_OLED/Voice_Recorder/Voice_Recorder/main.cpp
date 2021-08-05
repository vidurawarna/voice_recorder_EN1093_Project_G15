/*
 * Voice_Recorder.cpp
 *
 * Created: 6/5/2021 11:13:04 AM
 * Author : Vidura
 */ 

//>----------------- LIBRARIES -----------------<

#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <SD.h>
#include <SPI.h>
#include <Wire.h>
//#include <LCDScreen.h>
#include <SSD1306Ascii.h>
#include <SSD1306AsciiAvrI2c.h>


//>----------- PIN  CONFIGURATIONS -------------<

#define mic 0b0000
#define sdcard 10

//>----------------- CALCULATIONS -----------------<

#define lower_Byte(w) ((uint8_t) ((w) & 0xff))
#define higher_Byte(w) ((uint8_t) ((w) >> 8))
#define sbi(port,bit) port |= 1<<bit

//>-------------- common values -------------------<

//Audio file attributes
#define sampleRate 12500
#define byteRate (sampleRate/8)*monoStereo*8
#define monoStereo 1
#define dcOffset 127
#define maxFiles 15
#define lcdAddr 0x27 //Change this to 0x20 for proteus simulation,0x27 for real application
#define I2C_ADDRESS 0x3C

//for filters
#define bufflen 26
#define temp_buff_size 75
#define coslen 13

/*latest code takes 80microsec to read a single data - 12.5kHz
   takes 80microsec to read and output one data - 12.5kHz
   (Updated 2021/5/26 6.07pm)
*/

//variables for mode
char mode = 'i';
char mode_ = 'j';

//variables for frequency changing configuration
uint8_t freqScal = 1;

//variables to handle file operations
char tracks[maxFiles];
uint8_t files = 0;
uint8_t fcount = 0;
char fname_temp[7];


//LCD screen instance
//LCDScreen lcd(lcdAddr);

//OLED instance

SSD1306AsciiAvrI2c oled;

//functions used in recorder
char keyInput();

//functions for LCD
void firstLine(const char*);
void clrDisplay(const char*);
void secondLine(const char*);

//Recording,playing functions
void record();
void playTrack();
void checkChanges();
void getTrackList();
void nextTrack();
void previousTrack();
void checkDuplicates();
void deleteTrack();

//wave file creating functions
void makeWaveFile(File);
void finalizeWave(File);

//frequency modification functions
void pickFilter(char);
void convolve(int[],char[],uint8_t,int);
void sig_freqShift(char[]);

//IO functions
void initialize_Things();
uint8_t analog_in(int);


int main(void)
{	
	//This function is from arduino...need to edit
	initialize_Things();
	//sei();

	fname_temp[1] = '.';fname_temp[2] = 'W';fname_temp[3] = 'A';fname_temp[4] = 'V';

	//PORTD FOR KEYS
	DDRD = 0b00000000;
	PORTD = 0b11111111;
	
	//CONFIGURING PINS FOR ANALOG INPUT
	DDRC &= 0b11111110;	
	
	//CONFIGURING SPEAKER FOR OUTPUT
	DDRB |= (1<<DDB1);
	OCR1A = 0;
	
	//BEGIN THE LCD
	//lcd.begin();
	
	//beginning the OLED
  oled.begin(&Adafruit128x32, I2C_ADDRESS);
 // oled.displayRemap(true);
	
	
	//INITIALIZING THE SD CARD
	if (!SD.begin(sdcard))
	{
		clrDisplay("Error");
		while (1);
	}
	
	getTrackList();
	
	_delay_ms(1000);

    while (1) 
    {
		
		  //>-------------------------------< RECORD MODE (LEVEL 1)>--------------------------------<
		  if (mode == 's' && mode_ == 'j')
		  {
			record();
			getTrackList();
			mode = 'i';
		  }

		  //>--------------------------< ENTERING PLAYER MODE (LEVEL 1)>----------------------------<

		  if (mode == 'p' && mode_ == 'j')
		  {
			mode_ = 'k';
			mode = 'i';
			/*
			  This is the player mode
			  It loads the tracks in alphabetic order
			  Press 'Play/Stop' when a track is loaded to the player
			  Press 'Play/Stop' to stop playing
			  Press 'next' or 'previous' to toggle between tracks
			  Press 'record/stop' in track loaded mode to exit player mode
			*/
			if (files == 0)
			{
			  mode_ = 'j';
			  clrDisplay("No Tracks");
			  _delay_ms(1000);
			}
			else
			{
				clrDisplay("Ready to Play");
			  
			  _delay_ms(1000);
			  
			  fname_temp[0] = tracks[fcount];
			  secondLine(fname_temp);
			}
		  }
		  //>--------------------------< PLAYER MODE (LEVEL 2)>------------------------------------<
		  if (mode_ == 'k')
		  {
			  while (1)
			  {
				  char key_input = keyInput();
				  if (key_input)
				  {
					  mode = key_input;
					  break;
				  }
			  }
			if (mode == 'p')
			{
			  //Play the track
			  playTrack();
			  
			  clrDisplay("Ready to Play");

			}
			else if (mode == '>')
			{
			  //Load the next track
			  nextTrack();
			}
			else if (mode == '<')
			{
			  //load the previous track
			  previousTrack();
			}
			else if (mode == 's')
			{
			  //Exit from player mode
			  mode_ = 'j';
			  mode = 'i';
			}
			else if (mode == 'd')
			{
						  //This mode deletes the track loaded in payer
						  clrDisplay("Delete?");
						  secondLine("DELETE NO(Play)");
						  while (true)
						  {
							  char key = keyInput();
							  if (key && key == 'd')
							  {
								  deleteTrack();
								  clrDisplay("Deleted");
								  getTrackList();
								  
								  if(fcount == files){
									  fcount--;
								  }
								  
								  _delay_ms(1000);
								  break;
							  }
							  else if (key=='p')
							  {
								  clrDisplay("Not Deleted");
								  _delay_ms(1000);
								  break;
							  }
						  }

						  if (files == 0)
						  {
							  clrDisplay("No Tracks");
							  _delay_ms(1000);
							  mode_ = 'j';
						  }
						  else{
							  clrDisplay("Ready to Play");

							  fname_temp[0] = tracks[fcount];
						  }
			}
			secondLine(fname_temp);
			mode = 'i';
		  }
		  //>-------------------------< PAUSE MODE (LEVEL 1)>--------------------------------------<
		  if (mode == 'i' && mode_ == 'j')
		  {
			clrDisplay("Voice Recorder");
			while (1)
			{
				char key_input = keyInput();
				if (key_input)
				{
					mode = key_input;
					break;
				}
			}
		  }
    }
}

/*
   Definitions and implementations all the functions used in recorder
*/
//>-----------------------------------< KEYPAD FUNCTIONS >-------------------------------------<

char keyInput() {
  /*
    This function detects a key press and return the corresponding key
  */
  char k = 0;

  while (uint8_t m = ~PIND) {

    switch (m) {
      case 1: k = '_'; break;
      case 2: k = '*'; break;
      case 4: k = 's'; break;
      case 8: k = '<'; break;
      case 32: k = 'p'; break;
      case 64: k = '>'; break;
      case 128: k = 'd'; break;
      default: k = 0; break;
    }
    _delay_ms(300);
    PORTD = 0b11111111;

  }
  return k;
}

//END OF KEYPAD FUNCTIONS

//>--------------------------------< LCD DISPLAY FUNCTIONS >-----------------------------------<
void firstLine(const char *msg) {
  //Prints the string passed in the first line of the LCD display
  //lcd.setCursor(0, 0);
  //lcd.print(msg);
  oled.setFont(Arial_bold_14);
  oled.println(msg);
  
}

void clrDisplay(const char *msg) {
  //Clears the LCD and displays the msg in first line
  //lcd.clear();
  //lcd.setCursor(0, 0);
  //lcd.print(msg);
  oled.clear();
  oled.setFont(Arial_bold_14);
  oled.println(msg);
}

void secondLine(const char *msg) {
  //Prints the string passed in the second line of the LCD display
  //lcd.setCursor(0, 1);
  //lcd.print(msg);
  oled.setFont(ZevvPeep8x16);
  oled.println(msg);
}

//END OF LCD DISPLAY FUNCTIONS

//>-----------------------------< RECORD AND PLAY FUNCTIONS >----------------------------------<

void record() {
	 /*Used to record the data got from input into a file*/
	  checkDuplicates();
	  File test_File = SD.open(fname_temp, FILE_WRITE);

	  if (!test_File) {
		clrDisplay("Error");
	  }
	  else {
		clrDisplay("Recording");
		makeWaveFile(test_File);
		uint8_t pot_Read;

		while (true) {
		
		  pot_Read = analog_in(mic) + 64;
		
		  char key = keyInput();

		  if (key && key == 's') {
			break;
		  }

		  test_File.write(pot_Read);
		  //_delay_us(8);//16kHz
		  _delay_us(28);// 12.5kHz

		}
		finalizeWave(test_File);
		clrDisplay("Saved");
		_delay_ms(1000);
	  }
	  test_File.close();
}

void playTrack()
{
  /*This function reads data from the specified file and play*/

  checkChanges();//check for frequency change requirements
clrDisplay("Playing");

  File test_File = SD.open(fname_temp);

  if (!test_File) {
    // if the file didn't open, print an error:
    secondLine("Error");
    _delay_ms(1000);
  }
  else {
    test_File.seek(44);
    
    secondLine(fname_temp);

    //Check whether a frequency scale is set
    //>---------------------------< NORMAL OUTPUT >----------------------------------<

    if (freqScal == 0 || freqScal == 1) {
      while (test_File.available()) {

        OCR1A = test_File.read();
		
        _delay_us(40);  //Use this delay for 12.5KHz play
        //_delay_us(16);    //Use this delay for 16kHz play
        
		//****Comment both of delays for 24kHz play*******
        
        char key = keyInput();
        if (key && key == 'p') {
          break;
        }
      }
    }

    //>---------------------------< SCALED OUTPUT >----------------------------------<
    //Output for freaquency scaled track
    //Using down sampling

    else {
      uint8_t count = 1;
      while (test_File.available()) {

        char key = keyInput();
        if (key && key == 'p') {
          break;
        }

        if (count == 1) {
          //Accept the first sample among (# of samples=freqScal)
		  OCR1A = test_File.read();
          
          _delay_us(40);  //Use this delay for 12.5KHz play
          //_delay_us(16);    //Use this delay for 16kHz play
          //Comment both of delays for 24kHz play

        } else {
          test_File.read();//This is to neglet samples in between
        }

        count++;

        if (count == freqScal + 1) {//resetting the count
          count = 1;
        }
      }
    }
    // close the file:

	OCR1A = 0;
    secondLine("End of play");
    test_File.close();
	    
  }
  _delay_ms(1000);
   fname_temp[0] = tracks[fcount];fname_temp[1] = '.';fname_temp[2] = 'W';fname_temp[3] = 'A';fname_temp[4] = 'V';fname_temp[5] = NULL;
}

void checkChanges() {
  /*
     This function checks for frequency change requirements
  */
	clrDisplay("SCL M");
	
	char fsc =49;
	char fshift='X';
	char row[6] = {' ',fsc,' ',' ',fshift};
		
	while(true){
		char key_input = keyInput();
		if (key_input=='p')
		{
			freqScal = uint8_t(fsc) - 48;
			break;
		}
		else if(key_input=='>'){
			if(fshift=='X'){
				fshift='H';
			}
			else if(fshift=='H'){
				fshift='L';
			}
			else if(fshift=='L'){
				fshift='B';
			}
			else if(fshift=='B'){
				fshift='S';
			}
			else if(fshift=='S'){
				fshift='X';
			}
		}
		else if(key_input=='<'){
			if(fsc==51){
				fsc=48;
			}
			fsc++;
		}
		row[1]=fsc;row[4]=fshift;
		secondLine(row);
	}
	
	if(fshift!='X'){
		  clrDisplay("Processing");
		  pickFilter(fshift);
	}
}
//END OF RECORD AND PLAY FUNCTIONS

//>--------------------------------------< FILE HANDLING FUNCTIONS >--------------------------------------<

void getTrackList() {
  /*
     This function checks for files and make a list of available files
     Program only accept 15 tracks
     counts the number of files
  */
  char ASCIIcount = 65;
  files = 0;
  
  fname_temp[0] = ASCIIcount;
  
  uint8_t arrIndex = 0;
  while (true) {
    if (arrIndex == maxFiles || ASCIIcount == 91) {
      break;
    }
    if (SD.exists(fname_temp)) {
      tracks[arrIndex++] = fname_temp[0];
      files++;
    }
	fname_temp[0] = ++ASCIIcount;
  }
  for (uint8_t i = arrIndex; i < maxFiles; i++) {
    tracks[i] = '_';
  }
}

void nextTrack() {
  /*
     Checks tracks in order and returns the next track
  */
  fcount++;
  if (tracks[fcount] == '_') {
    fcount = 0;
  }
  fname_temp[0] = tracks[fcount];
}

void previousTrack() {
  /*
     Checks tracks in order and returns the previous track
  */
  if (fcount == 0) {
    fcount = files - 1;
  }
  else
  {
    fcount--;
  }
  fname_temp[0] = tracks[fcount];
}

void checkDuplicates() {

  /*This function checks if the new file to be made is existing,
    if does it generates a new name for the file*/
  char count = 65;

  fname_temp[0] = count;
  while (true) {
    if (SD.exists(fname_temp)) {
	  fname_temp[0] = ++count;
    } else {
      break;
    }
  }
}

void deleteTrack(){
	/*
		Checks for depending files of the current file and delete them.
	*/
	SD.remove(fname_temp);

	fname_temp[0] = 'S';fname_temp[1] = tracks[fcount];fname_temp[2] = '.';fname_temp[3] = 'W';fname_temp[4] = 'A';fname_temp[5] = 'V';
	if(SD.exists(fname_temp)){SD.remove(fname_temp);}
	fname_temp[0] = 'H';
	if(SD.exists(fname_temp)){SD.remove(fname_temp);}
	fname_temp[0] = 'L';
	if(SD.exists(fname_temp)){SD.remove(fname_temp);}
	fname_temp[0] = 'B';
	if(SD.exists(fname_temp)){SD.remove(fname_temp);}
		
	fname_temp[0] = tracks[fcount];fname_temp[1] = '.';fname_temp[2] = 'W';fname_temp[3] = 'A';fname_temp[4] = 'V';fname_temp[5] = NULL;
}

//END OF FILE HANDLING FUNCTIONS

//>------------------------------< FUNCTIONS FOR WAVE FILE CREATION >---------------------------------<

void makeWaveFile(File sFile) {
  /*
     This function creates the wave header file required
     All bytes should be in little endian format, except String values
  */

  sFile.write((uint8_t*)"RIFF    WAVEfmt ", 16);//Starting bytes of the wave header file
  uint8_t chunk[] = {16, 0, 0, 0, 1, 0, 1, 0, lower_Byte(sampleRate), higher_Byte(sampleRate)};
  /*
     chunk[]
     first 4 bytes: size of  previous data chunck
     next 2 bytes: Audio format (1 - PCM)
     next 2 byte: No of channels (Mono = 1, Stereo = 2) (in our case 1)
     last two are the first two bytes of sample rate
  */
  sFile.write((uint8_t*)chunk, 10);

  chunk[0] = 0; chunk[1] = 0; //end of sample rate bytes

  //byteRate = (sampleRate/8)*monoStereo*8;
  chunk[2] = lower_Byte(byteRate); chunk[3] = higher_Byte(byteRate); chunk[4] = 0; chunk[5] = 0; // byteRate

  //byte blockAlign = monoStereo * (bps/8);
  //this is always equal to 1 in 8bit PCM mono channel
  chunk[6] = 1; chunk[7] = 0; //BlockAlign

  chunk[8] = 8; chunk[9] = 0; //bits per sample

  sFile.write((uint8_t*)chunk, 10);
  sFile.write((uint8_t*)"data    ", 8);

}

void finalizeWave(File sFile) {
  /*
     This function finalizes the wave file
  */
  unsigned long fSize = sFile.size();

  fSize -= 8;
  sFile.seek(4);
  uint8_t chunk2[4] = {lower_Byte(fSize), higher_Byte(fSize), fSize >> 16, fSize >> 24};
  sFile.write(chunk2, 4);//Writing chunksize to 5 - 8 bytes in wave file

  sFile.seek(40);
  fSize -= 36 ;
  chunk2[0] = lower_Byte(fSize); chunk2[1] = higher_Byte(fSize); chunk2[2] = fSize >> 16; chunk2[3] = fSize >> 24;
  sFile.write((uint8_t*)chunk2, 4);//Writting num of samples to 41-44 bytes in wave file
}
//END OF WAVE FILE CREATE FUNCTIONS


//>--------------------------------------< FREQUENCY CHANGES >--------------------------------------<
void sig_freqShift(char tempName[]) {
		
		File out = SD.open("temp.bin", FILE_WRITE);
		File target = SD.open(tempName, FILE_READ);
		target.seek(44);

		uint8_t buff[bufflen];
		//int16_t cosWave12_5[coslen] = {10, 10, 9, 7, 5, 3, 1, -2, -4, -6, -8, -9, -10, -10, -9, -8, -6, -4, -2, 1, 3, 5, 7, 9, 10};
		int16_t cosWave12_5[coslen] =	{10,8,5,0,-4,-8,-9,-9,-6,-1,3,7,9};
		uint8_t count = 0;
		uint8_t buffCount = 0;

		while (target.available()) {

			buff[buffCount++] = (uint8_t)((int)(target.read() - dcOffset) * cosWave12_5[count++] / 10 + dcOffset);
			if (count == coslen )
			{
				count = 0;
			}
			if (buffCount == bufflen) {
				buffCount = 0;
				out.write((uint8_t*)buff, bufflen);
			}
		}

		out.close();
		target.close();

}

void pickFilter(char M){
	
	char tempName[6] = {tracks[fcount],'.','W','A','V'};
	fname_temp[0] = M;fname_temp[1] = tracks[fcount];fname_temp[2] = '.';fname_temp[3] = 'W';fname_temp[4] = 'A';fname_temp[5] = 'V';
	
	if(!SD.exists(fname_temp)){
		if(M=='S' || M=='H'){

			//int filter[11] ={-18,-47,-83,-121,-148,833,-148,-121,-83,-47,-18};//kaiser 100fc beta1.8
			int filter[11] =	{-18,-46,-83,-120,-148,834,-148,-120,-83,-46,-18};//kaiser 1000fc beta1.84
			if(M=='S'){
				sig_freqShift(tempName);
				convolve(filter,tempName,11,500);
			}
			else{
				convolve(filter,tempName,11,500);
			}			
		}
		
		else if(M=='B'){
			//int filter[26] ={0,0,0,5,19,29,10,-44,-106,-121,-50,77,181,181,77,-50,-121,-106,-44,10,29,19,5,0,0,0};//bandpass Bar-hann 
			//int filter[15] ={14,-10,-71,-127,-107,12,164,233,164,12,-107,-127,-71,-10,14};//kaiser beta3 good
			int filter[11] =	{-69,-128,-109,13,170,242,170,13,-109,-128,-69};//kaiser beta1.9
			convolve(filter,tempName,11,500);		
		}
		else if(M=='L'){
			//int filter[14] ={66,68,70,72,73,74,74,74,74,73,72,70,68,66};
			int filter[12] =	{37,57,77,96,111,119,119,111,96,77,57,37};//KAISER BETA=-2
			convolve(filter,tempName,12,500);
		}
	}
}

void convolve(int filter[],char tempName[],uint8_t filterlen,int divider) {
	/*
		Times that processing took:
		
			Band Pass: 1min 55sec for kaiser beta1.9
			Frequency shifting: 2min 45sec
			High pass: 1min 55sec
			Low pass: 1min 53sec
	*/
			
	uint8_t signal_in[filterlen];
	uint8_t temp_buff[temp_buff_size];
	float temp = 0;
	uint8_t temp_count = 0;
	File target;
			
	if(fname_temp[0]=='S'){target = SD.open("temp.bin", FILE_READ);}
	else{target = SD.open(tempName, FILE_READ);}
	File out = SD.open(fname_temp, FILE_WRITE);	
	makeWaveFile(out);
			
	unsigned long fSize = target.size();

	target.read(signal_in, filterlen);
	target.read(temp_buff, temp_buff_size);

	while (fSize) {

		if (temp_count == temp_buff_size) {
			target.read(temp_buff, temp_buff_size);
			temp_count = 0;
		}
		temp = dcOffset;

		for (uint8_t i = 0; i < filterlen - 1; i++) {
			temp += ((float(signal_in[i]) - dcOffset) * filter[i] / divider);
			signal_in[i] = signal_in[i + 1];
		}
		temp += ((float(signal_in[filterlen - 1]) - dcOffset) * filter[filterlen - 1] / divider);
		signal_in[filterlen - 1] = temp_buff[temp_count++];

		out.write(uint8_t(temp));

		fSize --;
	}
	finalizeWave(out);
	out.close();
	target.close();
			
	if(SD.exists("temp.bin")){SD.remove("temp.bin");}	
}
//END OF FREQUENCY CHANGES


//>--------------------------------------< IO FUNCTIONS >--------------------------------------<
uint8_t analog_in(int inputPin = 0000){
		
	ADMUX |= inputPin;
	
	ADCSRA = ADCSRA | (1 << ADSC);
	while(ADCSRA & (1 << ADSC));
	
	ADMUX &= 0b11110000;
	
	return ADCH;
}

void initialize_Things()
{
	// this needs to be called before setup() or some functions won't
	// work there
	sei();
	
	// on the ATmega168, timer 0 is also used for fast hardware pwm
	// (using phase-correct PWM would mean that timer 0 overflowed half as often
	// resulting in different millis() behavior on the ATmega8 and ATmega168)
	sbi(TCCR0A, WGM01);
	sbi(TCCR0A, WGM00);
	
	// set timer 0 prescale factor to 64
	// this combination is for the standard 168/328/1280/2560
	sbi(TCCR0B, CS01);
	sbi(TCCR0B, CS00);
	
	// enable timer 0 overflow interrupt
	sbi(TIMSK0, TOIE0);
	
	// timers 1 and 2 are used for phase-correct hardware pwm
	// this is better for motors as it ensures an even waveform
	// note, however, that fast pwm mode can achieve a frequency of up
	// 8 MHz (with a 16 MHz clock) at 50% duty cycle
	TCCR1B = 0;
	
	//select no-prescaling
	//sbi(TCCR1B, CS11);
	sbi(TCCR1B, CS10);
	
	//select the Wave form generation mode as FAST PWM
	//select the non-inverting mode
	sbi(TCCR1A, WGM10);
	sbi(TCCR1A, COM1A1);
	sbi(TCCR1B, WGM12);
	
	// set timer 2 prescale factor to 64
	sbi(TCCR2B, CS22);
	
	// configure timer 2 for phase correct pwm (8-bit)
	sbi(TCCR2A, WGM20);
	
	// set a2d prescaler(16) so we are inside the desired 50-200 KHz range.
	sbi(ADCSRA, ADPS2);
	//sbi(ADCSRA, ADPS1);
	//sbi(ADCSRA, ADPS0);
	
	// enable a2d conversions
	sbi(ADCSRA, ADEN);
	
	//set the reference voltage as AVCC
	//set the Left adjust result
	//keeping last 3bits as 0, because for the default pin selection as ADC0
	ADMUX = 0b01100000;
	
	// the bootloader connects pins 0 and 1 to the USART; disconnect them
	// here so they can be used as normal digital i/o; they will be
	// reconnected in Serial.begin()
	UCSR0B = 0;
}
//END OF IO FUNCTIONS