#ifndef _DATA_SERVER_H_
#define _DATA_SERVER_H_

#include <inttypes.h>
#include <stdio.h>
#include <avr/pgmspace.h>

void data_init();


/* Below procedures return amount of bytes 
 * written to buf
 */
uint16_t data_index(uint8_t* buf, uint16_t maxlen);

uint16_t data_style(uint8_t* buf, uint16_t maxlen);

uint16_t data_404(uint8_t* buf, uint16_t maxlen);

uint16_t data_input_too_long(uint8_t* buf, uint16_t maxlen);

uint16_t data_401(uint8_t* buf, uint16_t maxlen);

uint16_t data_http_header_html(uint8_t* buf, uint16_t maxlen);

uint16_t data_http_header_cookie(uint8_t* buf, uint16_t maxlen, uint8_t* auth_code);

uint16_t data_html_header(uint8_t* buf, uint16_t maxlen, uint8_t url_nest_level);



#endif //#ifndef _DATA_SERVER_H_
