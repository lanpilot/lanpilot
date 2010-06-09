#include "generator.h"

volatile uint32_t generator_tics;

void generator_initialize()
{
	DDRD |= 1 << PD5;	// Set OC1A as output
	TCCR1B = 1 << WGM13;	// Fast PWM mode
	TIMSK |= 1 << TOIE1; 	// enable interrupt
	generator_set_freq(55);	// some initial frequency
	sei();
}

void generator_start()
{
	TCCR1B |= 1 << CS10;
}

void generator_stop()
{
	TCCR1B &= ~((1 << CS10) | (1 << CS11) | (1 << CS12));
}

void generator_set_freq(uint8_t freq)
{
	ICR1 = ((F_CPU/2000)/freq)-1;
	OCR1A = ((((uint32_t)(F_CPU*IMPULSE_WIDTH)/2000)/freq)-1) ;
}

void generator_enable_output()
{
	TCCR1A = 1 << COM1A1;
}

void generator_disable_output()
{
	TCCR1A = 0;
}

void generator_emit_signal(uint8_t freq, uint16_t* data, uint16_t data_length)
{
	generator_set_freq(freq);
	generator_start();
	uint16_t i;
	uint8_t is_on = 0;
	for(i=0; i<data_length; ++i){
		if(is_on)
			generator_disable_output();
		else
			generator_enable_output();
		is_on = is_on ^ 0x01;
		generator_tics = 0;
		while(generator_tics < data[i]){};
	}
	generator_disable_output();
	generator_stop();
}

ISR( TIMER1_OVF_vect )
{
	generator_tics++;
}

