#include <avr/io.h>
#include <util/delay.h>
#include "tcpip/enc28j60.h"
#include "HTTPServer.h"



int main(void)
{
        /* enable PD2/INT0, as input */
        DDRD &= ~(1<<DDD2);

        /*initialize enc28j60*/
        _delay_loop_1(50);
        enc28j60Init(my_mac);
        enc28j60clkout(2); // change clkout from 6.25MHz to 12.5MHz
        _delay_loop_1(50);
        
        // LED
        /* enable PB1, LED as output */
        DDRB|= (1<<DDB1);

        /* set output to Vcc, LED off */
        PORTB|= (1<<PORTB1);

	/* Activate pullups on PD2, PD3 */
	PORTD |= (1 << PD2) | (1 << PD3);


        /* Magjack leds configuration, see enc28j60 datasheet, page 11 */
        // LEDB=yellow LEDA=green
        //
        // 0x476 is PHLCON LEDA=links status, LEDB=receive/transmit
        // enc28j60PhyWrite(PHLCON,0b0000 0100 0111 01 10);
        enc28j60PhyWrite(PHLCON,0x476);
        _delay_loop_1(50); // 12ms
        
        /* set output to GND, red LED on */
        PORTB &= ~(1<<PORTB1);

        http_server_run();

        return (0);
}
