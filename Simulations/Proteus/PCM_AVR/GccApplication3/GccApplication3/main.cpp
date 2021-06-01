/*
 * GccApplication3.cpp
 *
 * Created: 6/1/2021 12:43:13 PM
 * Author : nusha
 */ 
#define F_CPU 1000000UL
#include <avr/io.h>
#include <util/delay.h>

int analogRead(int inputPin = 0000){
	DDRC = 0b00000000;
	ADMUX = 0b01100000;
	ADCSRA = 0b10000100;
	
	ADMUX |= inputPin;
	
	ADCSRA = ADCSRA | (1 << ADSC);
	while(ADCSRA & (1 << ADSC));
	
	return ADCH;
}

	//this analog write function write on the pin OCA0
void analogWrite_config(){
	TCCR0A |= (1<<WGM00)|(1<<COM0A1)|(1<<WGM01);
	TCCR0B |= (1<<CS00);
	DDRD |= (1<<DDD6);
}
int main(void)
{
	analogWrite_config();
    while (1) 
    {
	
	OCR0A = analogRead();
    }
}

