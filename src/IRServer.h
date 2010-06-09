#ifndef _IR_SERVER_H_
#define _IR_SERVER_H_

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <avr/pgmspace.h>
#include "generator.h"
#include "HTTPServer.h" // this is required for a hack that
			// uses data[] as temporary uint16_t table
			// Here, it uses DATA_SIZE constant

#define TIMEOUT_VALUE 65000

void ir_init();

uint16_t ir_index(uint8_t* buf, uint16_t maxlen);

uint16_t ir_read(uint8_t* buf, uint16_t maxlen, uint8_t* data);

uint16_t ir_send(uint8_t* buf, uint16_t maxlen, uint8_t* data);


#endif //#ifndef _IR_SERVER_H_
