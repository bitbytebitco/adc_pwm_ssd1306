#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

ISR(TIMER0_OVF_vect){

	PORTB ^= (1 << PB0); 
}

int main(void)
{
	// PB0 as output, set value to HIGH
	DDRB |= (1<<PB0);
	PORTB |= (1 << PB0); 

	// init timer (normal port operation)
	TCCR0A |= (0 << COM0A1);
	TCCR0A |= (0 << COM0B1);
	TCCR0A |= (0 << COM0A0);
	TCCR0A |= (0 << COM0B0);

	// TOP = 0xFF, TOV occurs at MAX value
	TCCR0B |= (0 << WGM02); 
	TCCR0A |= (0 << WGM01);
	TCCR0A |= (0 << WGM00);

	// prescaler divide by 1024
	TCCR0B |= (1 << CS02); 
	TCCR0B |= (0 << CS01); 
	TCCR0B |= (0 << CS00); 

	// global interrupt enable
	//SREG |= (1 << 7);
	sei();

	// Timer0 interrupt enable
	TIMSK |= (1 << TOIE0);

	// set count to 0
	TCNT0 = 0; 

	while(1) {}

}
