#include <avr/io.h>
#include <stdio.h>
#include <util/delay.h>
#include <avr/pgmspace.h> //ssd1306
#include <avr/interrupt.h>
#include "i2c_attiny85_twi/i2c/i2c_primary.c"
#include "i2c_attiny85_twi/libs/ssd1306_attiny85.c"

volatile int Dac = 0;

ISR (TIMER1_OVF_vect) {
  OCR1A = Dac; // value compared against counter (TNCT1) used to control duty cycle 
}

void initADC(){
  ADMUX =
            (1 << ADLAR) |     // left shift result
            (0 << REFS1) |     // Sets ref. voltage to VCC, bit 1
            (0 << REFS0) |     // Sets ref. voltage to VCC, bit 0
            (0 << MUX3)  |     // use ADC2 for input (PB4), MUX bit 3
            (0 << MUX2)  |     // use ADC2 for input (PB4), MUX bit 2
            (1 << MUX1)  |     // use ADC2 for input (PB4), MUX bit 1
            (0 << MUX0);       // use ADC2 for input (PB4), MUX bit 0

  ADCSRA = 
            (1 << ADEN)  |     // Enable ADC 
            (1 << ADPS2) |     // set prescaler to 64, bit 2 
            (0 << ADPS1) |     // set prescaler to 64, bit 1 
            (0 << ADPS0);      // set prescaler to 64, bit 0  


}

void initPWM(){
	// set PB1 output
	DDRB |= (1<<PB1);
	PORTB |= (1<<PB1);
	// TIMER1 into PWM mode
	TCCR1 |= (1<< PWM1A) | (1<<COM1A0) | (1<<CS10);
	//OCR1A = 
}

int main(void)
{
	initADC();
	initPWM();
	
	// global interrupt enable
	sei();

	// Timer0 interrupt enable
	//TIMSK |= (1 << OCIE0A) | (1<<TOIE1);
	TIMSK |= (1<<TOIE1);


	// ssd1306
	i2c_init();

	ssd1306_init();
	ssd1306_send_progmem_multiple_data(default_image_length, image_1); // ssd1306 raw example: show an image
	_delay_ms(1000);
	ssd1306_send_progmem_multiple_data(default_image_length, image_2);


	while(1) {
		ADCSRA |= (1 << ADSC);         // start ADC measurement
		while (ADCSRA & (1 << ADSC) ); // wait till conversion complete 

		//_delay_ms(10);
	
		// write ADC
		Dac = ADCH; 
	}

}
