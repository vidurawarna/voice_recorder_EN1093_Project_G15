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
#include <LCDScreen.h>


//>----------------- CALCULATIONS -----------------<

#define lower_Byte(w) ((uint8_t) ((w) & 0xff))
#define higher_Byte(w) ((uint8_t) ((w) >> 8))
#define sbi(port,bit) port |= 1<<bit

//>----------- PIN  CONFIGURATIONS -------------<

/*
 SD card attached to SPI bus as follows:
	+-------------+----------------------+------------+  
	| Connection  | Arduino Uno/Nano pin | ATmega328p |
	+-------------+----------------------+------------+
    | MOSI        | pin 11               | PB3        |
	+-------------+----------------------+------------+
    | MISO        | pin 12               | PB4        |
	+-------------+----------------------+------------+
    | CLK         | pin 13               | PB5        |
	+-------------+----------------------+------------+
    | CS          | pin 10               | PB2        |
	+-------------+----------------------+------------+
*/

#define mic 0b0000
#define ScalePOT 0b0011
#define shiftEnhancePOT 0b0001
#define sdcard 10

//>-------------- common values ----------------<

//Audio file attributes
#define sampleRate 12500
#define byteRate (sampleRate/8)*monoStereo*8
#define monoStereo 1
#define maxFiles 15
#define lcdAddr 0x27 //Change this to 0x20 for proteus simulation,0x27 for real application

//for filters
#define bufflen 25
#define filterlen 11
#define temp_buff_size 100

/*latest code takes 80microsec to read a single data - 12.5kHz
   takes 80microsec to read and output one data - 12.5kHz
   (Updated 2021/5/26 6.07pm)
*/

//variables for mode
char mode = 'i';
char mode_ = 'j';

//variables for frequency changing configuration
uint8_t freqScal = 1;
bool shift = false;
uint8_t enhance = false;

//variables to handle file operations
char tracks[maxFiles];
uint8_t files = 0;
uint8_t fcount = 0;
char fname_temp[7];


//LCD screen instance
LCDScreen lcd(lcdAddr);

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

//wave file creating functions
void makeWaveFile(File);
void finalizeWave(File);

//frequency modification functions
void convolve();
bool sig_freqShift(char[]);

//IO functions
void initialize_Things();
uint8_t analog_in(int);
//void analogWrite_config();
//void analogRead_config();


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
	DDRC &= 0b11110100;	
	//analogRead_config();
	
	//CONFIGURING SPEAKER FOR OUTPUT
	DDRB |= (1<<DDB1);
	//analogWrite_config();
	OCR1A = 0;
	
	//BEGIN THE LCD
	lcd.begin();
	
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
		//>------------------------------------< KEY INPUT >--------------------------------------<

		  
		  

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
			  
			  //fname_temp = String(tracks[fcount]) + ".WAV";
			  //setFileName(tracks[fcount],"",".WAV");
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
			  
			  //setFileName(tracks[fcount],"",".WAV");
			  //secondLine(fname_temp);
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
			  //secondLine("DELETE");
			  while (true)
			  {
				char key = keyInput();
				if (key && key == 'd')
				{
				  SD.remove(fname_temp);
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
				  //fname_temp = String(tracks[fcount]) + ".WAV";
				  //setFileName(tracks[fcount],"",".WAV");
				  fname_temp[0] = tracks[fcount];
				  //secondLine(fname_temp);
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

  if (uint8_t m = ~PIND) {

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
  lcd.setCursor(0, 0);
  lcd.print(msg);
}

void clrDisplay(const char *msg) {
  //Clears the LCD and displays the msg in first line
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(msg);
}

void secondLine(const char *msg) {
  //Prints the string passed in the second line of the LCD display
  lcd.setCursor(0, 1);
  lcd.print(msg);
}

//END OF LCD DISPLAY FUNCTIONS

//>-----------------------------< RECORD AND PLAY FUNCTIONS >----------------------------------<

void record() {
	 /*Used to record the data got from input into a file*/
	  checkDuplicates();
	  File test_File = SD.open(fname_temp, FILE_WRITE);

	  if (!test_File) {
		clrDisplay("Error");
		_delay_ms(1000);
	  }
	  else {
		clrDisplay("Recording");
		makeWaveFile(test_File);
		uint8_t pot_Read;

		while (true) {
		  //t = micros();
		
		  pot_Read = analog_in(mic);
		
		  char key = keyInput();

		  if (key && key == 's') {
			break;
		  }

		  test_File.write(pot_Read);
		  _delay_us(16);
		  //t = micros() - t;
		  //clrDisplay(String(t));
		  //_delay_ms(1000);
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

  if (shift)
  {
    secondLine("Processing");
    convolve();
  }

  File test_File = SD.open(fname_temp);

  if (!test_File) {
    // if the file didn't open, print an error:
    secondLine("Error");
    _delay_ms(1000);
  }
  else {
    test_File.seek(44);
    clrDisplay("Playing");
    secondLine(fname_temp);

    //Check whether a frequency scale is set
    //>---------------------------< NORMAL OUTPUT >----------------------------------<

    if (freqScal == 0 || freqScal == 1) {
      while (test_File.available()) {
        //t = micros();
        OCR1A = test_File.read();
		
        _delay_us(40);  //Use this delay for 12.5KHz play
        //_delay_us(20);    //Use this delay for 16kHz play
        
		//****Comment both of delays for 24kHz play*******
        
        char key = keyInput();
        if (key && key == 'p') {
          break;
        }

        //Serial.println(micros() - t);
        //t = micros() - t;
        //clrDisplay(String(t));
        //_delay_ms(1000);;
      }
    }

    //>---------------------------< SCALED OUTPUT >----------------------------------<
    //Output for freaquency scaled track
    //Using down sampling

    else {
      uint8_t count = 1;
      while (test_File.available()) {
        //t = micros();
        char key = keyInput();
        if (key && key == 'p') {
          break;
        }

        if (count == 1) {
          //Accept the first sample among (# of samples=freqScal)
		  OCR1A = test_File.read();
          
          _delay_us(40);  //Use this delay for 12.5KHz play
          //_delay_us(20);    //Use this delay for 16kHz play
          //Comment both of delays for 24kHz play
          
          //Serial.println(micros() - t);
          //t = micros() - t;
          //clrDisplay(String(t));
          //delay(1000);
        } else {
          test_File.read();//This is to neglet samples in between
        }

        count++;

        if (count == freqScal + 1) {//resetting the count
          count = 1;
        }
        //Serial.println(micros() - t);
        //        t = micros() - t;
        //        clrDisplay(String(t));
        //        delay(1000);
      }
    }
    // close the file:
    //analogWrite(speaker, 0);
	OCR1A = 0;
    secondLine("End of play");
    test_File.close();

    if (shift || enhance) {
      //fname_temp = String(tracks[fcount]) + ".WAV";
	  //setFileName();

	  fname_temp[0] = tracks[fcount];fname_temp[1] = '.';fname_temp[2] = 'W';fname_temp[3] = 'A';fname_temp[4] = 'V';fname_temp[5] = NULL;
      shift = false;
      enhance = 0;
    }

    _delay_ms(1000);
  }
}

void checkChanges() {
  /*
     This function checks for frequency change requirements
  */

	uint8_t fsc = analog_in(ScalePOT);
	uint8_t fshift = analog_in(shiftEnhancePOT);

  if (fsc < 90) {
    freqScal = 1;
  }
  else if (fsc < 180) {
    freqScal = 2;
  }
  else {
    freqScal = 3;
  }
  
  if(fshift==0){
	  shift = false;
	  enhance = 0;
  }	
  else if (fshift < 64) {
    shift = true;
    enhance = 0;
  }
  else if (fshift < 128) {
    shift = false;
    enhance = 1;
  }
  else if(fshift < 192){
    shift = false;
    enhance = 2;
  }
  else{
	shift = false;
	enhance = 3;  
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
  
  //fname_temp = String(ASCIIcount) + ".WAV";
  //setFileName(ASCIIcount,"",".WAV");
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
    //fname_temp = String(++ASCIIcount) + ".WAV";
	//setFileName(++ASCIIcount,"",".WAV");
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
  //fname_temp = String(tracks[fcount]) + ".WAV";
  //setFileName(tracks[fcount],"",".WAV");
  fname_temp[0] = tracks[fcount];
  //secondLine(fname_temp);
  //Serial.println(count);
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
  //fname_temp = String(tracks[fcount]) + ".WAV";
  //setFileName(tracks[fcount],"",".WAV");
  fname_temp[0] = tracks[fcount];
  //secondLine(fname_temp);
  //Serial.println(count);
}

void checkDuplicates() {

  /*This function checks if the new file to be made is existing,
    if does it generates a new name for the file*/
  char count = 65;
  //fname_temp = String(count) + ".wav";
  //setFileName(count);
  fname_temp[0] = count;
  while (true) {
    if (SD.exists(fname_temp)) {
      //fname_temp = String(++count) + ".wav";
	  //setFileName(++count);
	  fname_temp[0] = ++count;
    } else {
      break;
    }
  }
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


//>--------------------------------------< FREQUENCY SHIFTING >--------------------------------------<
bool sig_freqShift(char tempName[]) {
	bool needConvolve;
//"S" + String(fname_temp[0]) + ".bin"
	if (!(SD.exists(tempName))) {
		needConvolve = true;
		File out = SD.open(tempName, FILE_WRITE);
		File target = SD.open(fname_temp, FILE_READ);
		target.seek(44);

		uint8_t buff[bufflen];
		int16_t cosWave12_5[25] = {10, 10, 9, 7, 5, 3, 1, -2, -4, -6, -8, -9, -10, -10, -9, -8, -6, -4, -2, 1, 3, 5, 7, 9, 10};
		uint8_t count = 0;
		uint8_t buffCount = 0;

		while (target.available()) {

			buff[buffCount++] = (uint8_t)((int)(target.read() - 127) * cosWave12_5[count++] / 10 + 127);
			if (count == 25 )
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
	else{
		needConvolve = false;
	}
	
	fname_temp[0] = 'S';fname_temp[1] = tracks[fcount];fname_temp[2] = '.';fname_temp[3] = 'W';fname_temp[4] = 'A';fname_temp[5] = 'V';

	return needConvolve;
}


void convolve() {
			
	if(shift && enhance==0){
		
		char tempName[7] = {'S',tracks[fcount],'.','B','I','N'};
		
		if(sig_freqShift(tempName)){
		
			int filter[filterlen] = {0, 1, 5, -4, -48, 920, -48, -4, 5, 1, 0};
	
			uint8_t signal_in[filterlen];
			uint8_t temp_buff[temp_buff_size];
			float temp = 0;
			uint8_t temp_count = 0;

	
			File out = SD.open(fname_temp, FILE_WRITE);
			makeWaveFile(out);
			File target = SD.open(tempName, FILE_READ);

			unsigned long fSize = target.size();

			target.read(signal_in, filterlen);
			target.read(temp_buff, temp_buff_size);

			while (fSize) {
				//t = micros();
				if (temp_count == temp_buff_size) {
					target.read(temp_buff, temp_buff_size);
					temp_count = 0;
				}
				temp = 127;
				//temp_ = 0;

				for (uint8_t i = 0; i < filterlen - 1; i++) {
					temp += ((float(signal_in[i]) - 127) * filter[i] / 1000);//570
					signal_in[i] = signal_in[i + 1];
				}
				temp += ((float(signal_in[filterlen - 1]) - 127) * filter[filterlen - 1] / 1000);//570
				signal_in[filterlen - 1] = temp_buff[temp_count++];


				//temp_ = byte(temp + 127) ;
				out.write(uint8_t(temp));
				//Serial.println(String(micros()-t));
				fSize --;


			}
			finalizeWave(out);
			out.close();
			target.close();
			//Serial.println("stop");
		}
	}
}
//END OF FREQUENCY SHIFTING


//>--------------------------------------< IO FUNCTIONS >--------------------------------------<
uint8_t analog_in(int inputPin = 0000){
		
	ADMUX |= inputPin;
	
	ADCSRA = ADCSRA | (1 << ADSC);
	while(ADCSRA & (1 << ADSC));
	
	ADMUX &= 0b11110000;
	
	return ADCH;
}

/*void analogWrite_config(){

	//TCCR1A |= ((1<<WGM10) | (1<<COM1A1));
	//TCCR1B |= ((1<<WGM12) | (1<<CS10));
	
	//Setting the timers for fast pwm mode
	TCCR1A |= ((1<<WGM10) | (1<<COM1A1));
	TCCR1A &= 0b10111101;
	TCCR1B |= ((1<<WGM12) | (1<<CS10));
	TCCR1B &= 0b11101001;
	
	//TCCR1B = (TCCR1B & 0b11111000) | 0x01;
	
}

void analogRead_config(){
	
	//set the reference voltage as AVCC
	//set the Left adjust result
	//keeping last 3bits as 0, because for the default pin selection as ADC0
	ADMUX = 0b01100000;
	
	//enable the ADC
	//set the ADC pre scaler as 16
	ADCSRA = 0b10000100;
}
*/
//END OF IO FUNCTIONS

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