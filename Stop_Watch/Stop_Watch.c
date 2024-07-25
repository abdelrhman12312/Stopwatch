#include<avr/io.h>
#include<util/delay.h>
#include <avr/interrupt.h>

unsigned int Digit[6];
unsigned int number = 0 ;
unsigned char i = 0 ;
unsigned int sec = 0;
unsigned int min = 0;
unsigned int hour = 0;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void timer1_init() {
	TCCR1A = (1 << FOC1A);
	TCCR1B |=(1<<CS10) | (1<<CS12); // prescaler of 1024
	TCCR1B |= (1<<WGM12);           // the TOP value is in OCR1A
    	TCNT1 = 0;
    	OCR1A = 977;
    	TIMSK |= (1 << OCIE1A);
    	SREG |= (1<<7);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
void External_INTERRUPTS_init() {

	MCUCR |= (1<<ISC01);             //  FALING EDGE CONFIG FOR INT0
	MCUCR |= (1<<ISC10) | (1<<ISC11); // RASING EDGE CONFIG FOR INT1
	MCUCSR &=~(1<<ISC2);              //FALLING EDGE CONFIG FOR INT2
	GICR |= (1<<INT0) | (1<<INT1) | (1<<INT2);
}


int main() {


	External_INTERRUPTS_init();
	timer1_init();

	DDRC  |= 0x0F; // the first Four bits are inputs
	PORTC &= 0xF0; // init with a statrting val of 0
	DDRA  |= 0x7F; // the first 7 bits  in port A  are outputs

	DDRD &=~(1<<PD2); // port D bit 2 , 3 as INPUTS
	DDRD &=~(1<<PD3);
	DDRB &=~(1<<PB2);

	PORTD |= (1 << PD2); // internal pull-ups
	PORTB |= (1 << PB2);
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
	while(1)  {

    Digit[5] = sec%10;
    Digit[4] = sec/10;
    Digit[3] = min%10;
    Digit[2] = min/10;
    Digit[1] = hour%10;
    Digit[0] = hour/10;

     for(i = 0 ; i<6; i++) {
    	 PORTA |= (1<<i);
    	 PORTC = Digit[i];
    	 _delay_ms(3);
    	 PORTA = 0 ;
     }

	}

}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
ISR (TIMER1_COMPA_vect) {
	sec++;
	if(sec==60) {
		sec =0;
		min++;
	}
	if(min==60) {
		min=0;
		hour++;
	}
	if(hour==99) {
		sec=0;
		min=0;
		hour=0;
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
ISR(INT0_vect) { //RESET

		TCNT1 = 0;
		sec = 0;
		min = 0;
		hour = 0;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
ISR(INT1_vect) { // PAUSE
	TCCR1B &=(~(1<<CS10)) & (~(1<<CS12));
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
ISR(INT2_vect) { //RESUME
	TCCR1B |=(1<<CS10) | (1<<CS12);
}
