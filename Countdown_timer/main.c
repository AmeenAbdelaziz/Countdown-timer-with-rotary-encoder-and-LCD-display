#define F_CPU 16000000
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>

#define DDR_LED DDRA
#define PORT_LED PORTA
#define SW0LED 0
#define SW1LED 1
#define SW2LED 2
#define SW3LED 3
#define SW4LED 4
#define SW5LED 5
#define SW6LED 6
#define SW7LED 7

#define SW_Pin PINK
#define SW0 0
#define SW1 1
#define SW2 2
#define SW3 3
#define SW7 7

volatile uint16_t counter=0;
volatile bool state = false;
bool timerStart = false;

void addVal(uint8_t val){
	uint8_t addValue=0;
	switch(val){
		case 0b1:addValue=1;break;		//0.25 sec
		case 0b10:addValue=4;break;		//1 sec
		case 0b100:addValue=40;break;	//10sec
	}
	
	
	
	if (counter<=156)   // Hier müsste es 80 (sprich 20 Seconden) statt 156 stehen.
						// Aber wenn es hier 80 steht, kann der Timer kommischerweise nur 10 Seconden zählen. 
						// Ich bin auf die Zahl 156 durch Try and Error gekommen.
	{
		counter=counter+addValue;
		PORT_LED=~val;
	}
	
	else{
		timerStart = true;
	}
	
	if (val==0b1000)
	{counter=0;
		PORT_LED=~val;
		timerStart = false;
	}
	while(SW_Pin==val);
	_delay_ms(500);
	PORT_LED=0xFF;
}

int main(void)
{

	DDR_LED = 0xFF;
	PORT_LED = 0xFF;		//all led are off

	TCCR1B|=(1<<CS12);
	TCNT1=57723;
	TIMSK1|=(1<<TOV1);
	sei();


	
	while (1)
	{
		if (((~SW_Pin)&0b1111)&&!state)
		{
			addVal((~SW_Pin)&0b1111);
			
		}
		if ((!(SW_Pin&(1<<SW7)))&(!timerStart))
		{
			PORT_LED&=~(1<<SW7LED);
			while(!(SW_Pin&(1<<SW7)));
			PORT_LED=0xFF;
			state=!state;
		}
	}
}
ISR(TIMER1_OVF_vect){

	if (state)
	{ if (counter>0)
		{counter--;
			PORT_LED^=(1<<SW0LED)|(1<<SW1LED)|(1<<SW2LED)|(1<<SW3LED)|(1<<SW4LED)|(1<<SW5LED)|(1<<SW6LED)|(1<<SW7LED);
		}

		if (counter<=0)
		{
			PORT_LED=0xFF; // off
			state=!state;
			timerStart = false;
		}
	}
	TCNT1=57723;
}
