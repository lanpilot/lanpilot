#ifndef _GENERATOR_H_
#define _GENERATOR_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <inttypes.h>


// Defines PWM impulse width
#define IMPULSE_WIDTH 0.3

// This variable will be incremented every time
// there is timer interrupt, that is at
// freq frequency - set it with call to
// generator_set_freq(freq)
extern volatile uint32_t generator_tics;


void generator_initialize();

void generator_start();

void generator_stop();

void generator_set_freq(uint8_t freq);

void generator_enable_output();

void generator_disable_output();

void generator_emit_signal(uint8_t freq, uint16_t* data, uint16_t data_length);

#endif // #ifndef _GENERATOR_H_
