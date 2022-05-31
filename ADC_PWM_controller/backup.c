#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "ssd1306.h"

volatile uint16_t tx_shift_reg = 0;
/*ISR(TIMER0_COMPA_vect){

	PORTB ^= (1 << PB0); 
}*/


volatile int Dac = 0;
volatile int Cycle = 0;

ISR (TIMER1_OVF_vect) {
  OCR1A = Dac; // value compared against counter (TNCT1) used to control duty cycle 
}

ISR(TIMER0_COMPA_vect )
{
   //output LSB of the TX shift register at the TX pin
   if( tx_shift_reg & 0x01 )
   {
      //TX_PORT |= (1<<TX_PIN); Th
	PORTB |= (1 << PB0); 
   }
   else
   {
      //TX_PORT &=~ (1<<TX_PIN);
	
	PORTB &=~(1 << PB0); 
   }
   //shift the TX shift register one bit to the right
   tx_shift_reg = (tx_shift_reg >> 1);
   //if the stop bit has been sent, the shift register will be 0
   //and the transmission is completed, so we can stop & reset timer0
   if(!tx_shift_reg)
   {
      TCCR0B = 0;
      TCNT0 = 0;
   }
}

void UART_tx(char character)
{
   //if sending the previous character is not yet finished, return
   //transmission is finished when tx_shift_reg == 0
   if(tx_shift_reg){return;}
   //fill the TX shift register witch the character to be sent and the start & stop bits
   tx_shift_reg = (character<<1);
   tx_shift_reg &= ~(1<<0); //start bit
   tx_shift_reg |= (1<<9); //stop bit
   //start timer0 with a prescaler of 8
   TCCR0B = (1<<CS01);
}

void UART_tx_str(char* string){
    while( *string ){
        UART_tx( *string++ );
        //wait until transmission is finished
        while(tx_shift_reg);
    }
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

	// PB0 as output, set value to HIGH
	DDRB |= (1<<PB0);
	PORTB |= (1 << PB0); 

	// init TIMER0 (normal port operation)
	TCCR0A |= (0 << COM0A1);
	TCCR0A |= (0 << COM0B1);
	TCCR0A |= (0 << COM0A0);
	TCCR0A |= (0 << COM0B0);

	// set to Compare Match  
	TCCR0B |= (0 << WGM02); 
	TCCR0A |= (1 << WGM01);
	TCCR0A |= (0 << WGM00);

	// prescaler divide by 8
	TCCR0B |= (0 << CS02); 
	TCCR0B |= (1 << CS01); 
	TCCR0B |= (0 << CS00);

	// (8 * 1000000)/8 = 1000000 Hz	
	// T = 1/f --> T = 1/1000000 = 1 uS 
	// // /

	// global interrupt enable
	sei();

	// Timer0 interrupt enable
	TIMSK |= (1 << OCIE0A) | (1<<TOIE1);
	
	// interrupt at 103 to acheive 9600 baud rate (104 uS)
	// 1/9600 = 104 uS
	
	OCR0A = 206;

	// set count to 0
	TCNT0 = 0; 

	while(1) {
		ADCSRA |= (1 << ADSC);         // start ADC measurement
		while (ADCSRA & (1 << ADSC) ); // wait till conversion complete 

		//UART_tx_str("ABCDE");
		//_delay_ms(50);
		UART_tx(ADCH);
		_delay_ms(10);
	
		// ADC value to PWM output PB1	
		//OCR1A = ADCH;

		// write ADC
		Dac = ADCH; 
	}

}
