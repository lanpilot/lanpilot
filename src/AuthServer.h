#ifndef _AUTH_SERVER_H_
#define _AUTH_SERVER_H_

#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <avr/pgmspace.h>

#define MAX_USER_NAME 8
#define MAX_USER_PASSWORD 8
#define COOKIE_LENGTH 8


void auth_init();

uint8_t auth_can_user_access(uint8_t service_num, uint8_t procedure_num, uint8_t* auth_cookie);

uint16_t auth_login_bar(uint8_t* buf, uint16_t maxlen, uint8_t* auth_cookie, uint8_t url_nest_level);

uint16_t auth_login(uint8_t* buf, uint16_t maxlen, uint8_t* data);

uint16_t auth_logout(uint8_t* buf, uint16_t maxlen, uint8_t* auth_cookie);

uint8_t* auth_set_cookie(uint8_t* data);

uint16_t auth_set_cookie_redirect(uint8_t* buf, uint16_t maxlen);

uint16_t auth_index(uint8_t* buf, uint16_t maxlen);

uint16_t auth_edit(uint8_t* buf, uint16_t maxlen, uint8_t* data);



#endif //#ifndef _AUTH_SERVER_H_
