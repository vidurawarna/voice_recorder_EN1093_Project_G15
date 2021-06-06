/*
 * GccApplication4.cpp
 *
 * Created: 6/3/2021 7:19:43 PM
 * Author : nusha
 */ 

#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>


void analogRead_config(){
	//set all the port C pins as output because port c contains all the ADC pins
	DDRC = 0b00000000;
	
	//set the reference voltage as AVCC
	//set the Left adjust result
	//keeping last 3bits as 0, because for the default pin selection as ADC0
	ADMUX = 0b01100000;
	
	//enable the ADC
	//set the ADC pre scaler as 16
	ADCSRA = 0b10000100;
}
int analogRead(int inputPin = 0000){

	//change or select the ADC pin
	ADMUX |= inputPin;
	
	//start the ADC conversion
	ADCSRA = ADCSRA | (1 << ADSC);
	
	//wait until the ADSC bit has been cleared
	while(ADCSRA & (1 << ADSC));
	
	//this will return the 8-bit value
	return ADCH;
}

//this analog write function write on the pin OCA0
void analogWrite_config(){
	//select the Wave form generation mode as FAST PWM
	//select the non-inverting mode
	TCCR0A |= (1<<WGM00)|(1<<COM0A1)|(1<<WGM01);
	
	//select no-prescaling
	TCCR0B |= (1<<CS00);
	
	//set OCA0 pin as a output
	DDRD |= (1<<DDD6);
}
void analogWrite_config2(){
	TCCR1A |= (1<<WGM10) | (1<<COM1A1);
	TCCR1B |= (1<<WGM12) | (1<<CS10);
	
	DDRB |= (1<<DDB1);
}
int main(void)
{
	analogRead_config();
	analogWrite_config2();
	while (1)
	{
		
		OCR1A = analogRead();
	}
}


