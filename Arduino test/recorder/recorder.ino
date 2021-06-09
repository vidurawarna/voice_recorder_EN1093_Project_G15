#include "configuration.h"

//variables for modes
char mode = 'i';
char mode_ = 'j';

//variables for frequency changing configuration
byte freqScal = 1;
bool shift = false, enhance = false;

//variables to handle file operations
byte tracks[maxFiles];
byte files = 0;
byte fcount = 0;
String fname_temp;


//LCD screen instance
LCDScreen lcd(lcdAddr);

//Variables for keypad
//int realVals[8] = {501, 594, 557, 504, 103,  363, 419, 292};
//char keys[8] = {'r', '<', 'p', '>', 's',  'd', 'm', 'o'};

long t;

//>---------------------------------------- INSTRUCTIONS FOR THE PLAYER --------------------------------------------------<
/*
     different mode and  mode_ variables keeps two mode states -> (When in player mode there are lot of functions to handle, therfore two mode levels are created)
     mode = 's' for record -> (press 'record/stop' to start/stop recording)      #This is mode level 1 called mode_='j'
     mode = 'p' for player mode -> (press 'play/stop'  to enter player mode when you are in pause mode; first of all this loads the first track to palyer)      #This is mode level 2 called mode_='k'
          toggle the tracks by pressing 'previous' or 'next' and press 'play/stop' to start playing
          press 'play/stop' again to stop playing
          press 'record/stop' to exit the player mode
     mode = 'i' for pause state (In this mode "Voice Recorder" will be displayed in screen)
*/
//END OF INSTRUCTIONS ----------------------------------------------------------------------------------------------------<

//Initializing things
void setup()
{
  //CONFIGURE ANALOD READ FOR FASTER READINGS
  //  ADCSRA |= (1 << ADPS2);
  //  ADCSRA &= ~(1 << ADPS1);
  //  ADCSRA &= ~(1 << ADPS0);

  ADCSRA &= ~(1 << ADPS2);
  ADCSRA |= (1 << ADPS1);
  ADCSRA |= (1 << ADPS0);

  //PORTD FOR KEYS
  DDRD = 0b00000000;
  PORTD = 0b11111111;

  //CONFIGURING PINS FOR INPUT
  pinMode(mic, INPUT);
  //pinMode(keypadPin, INPUT);
  pinMode(ScalePOT, INPUT);
  pinMode(shiftEnhancePOT, INPUT);
  
  //CONFIGURING SPEAKER FOR OUTPUT
  pinMode(speaker, OUTPUT);
  setPwmFrequency(speaker, 1); //function for setting PWM frequency

  //Serial.begin(9600);
//tt = "hello";
  lcd.begin();
  
//  lcd.print({'h','e','l','l','o','!'});
  //delay(1000);

  if (!SD.begin(sdcard))
  {
    clrDisplay("Error");
    while (1);
  }

  getTrackList();
  //t = millis();
  //convolve();
  //Serial.println(String(millis()-t));
  //secondLine(String(millis()-t));
//  secondLine("Welcome");

  delay(1000);
}


void loop()
{
  //>------------------------------------< KEY INPUT >--------------------------------------<

  char key_input = keyInput();
  if (key_input)
  {
    mode = key_input;
  }

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
      It loads the tracks in alphebetic order
      Press 'Play/Stop' when a track is loaded to the player
      Press 'Play/Stop' to stop playing
      Press 'next' or 'previous' to toggle between tracks
      Press 'record/stop' in track loaded mode to exit player mode
    */
    if (files == 0)
    {
      mode_ = 'j';
      // mode = 'i';
      clrDisplay("No Tracks");
      delay(1000);
    }
    else
    {
      clrDisplay("Ready to Play");
      delay(1000);
      fname_temp = String(char(tracks[fcount])) + ".WAV";
      secondLine(fname_temp);
    }
  }
  //>--------------------------< PLAYER MODE (LEVEL 2)>------------------------------------<
  if (mode_ == 'k')
  {
    if (mode == 'p')
    {
      //Play the track
      playTrack();
      clrDisplay("Ready to Play");
      delay(200);
      secondLine(fname_temp);
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
          delay(1000);
          break;
        }
        else if (key=='p')
        {
          clrDisplay("Not Deleted");
          delay(1000);
          break;
        }
      }
      if (files == 0)
      {
        clrDisplay("No Tracks");
        delay(1000);
        mode_ = 'j';
        //mode = 'i';
      }
      if(fcount == files){
        fcount--;
      }
      clrDisplay("Ready to Play");
      fname_temp = String(char(tracks[fcount])) + ".WAV";
      secondLine(fname_temp);
    }
    mode = 'i';
  }
  //>-------------------------< PAUSE MODE (LEVEL 1)>--------------------------------------<
  if (mode == 'i' && mode_ == 'j')
  {
    clrDisplay("Voice Recorder");

    while (true)
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
