#ifndef _SERVICES_SERVER_H_
#define _SERVICES_SERVER_H_

#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <avr/pgmspace.h>
#include "AuthServer.h"
#include "DataServer.h"
#include "IRServer.h"



/***************************** PROCEDURES *****************************/
void services_init();

uint16_t services_process_user_request(uint8_t *_buf, uint16_t _buf_len, 
					uint8_t *_service, uint8_t *_procedure, 
					uint8_t *_auth_cookie, uint8_t *_data,
					uint8_t _is_data_ok);


/***************************** DATA *****************************/


#define SERVICE_ROOT 1
#define SERVICE_DATA 2
#define SERVICE_AUTH 3
#define SERVICE_IR   4

#define service_root_name PSTR("")
#define service_data_name PSTR("data")
#define service_auth_name PSTR("auth")
#define service_ir_name PSTR("ir")


#define SERVICE_ROOT_ROOT 1
#define service_root_root PSTR("")

#define SERVICE_DATA_ROOT 1
#define SERVICE_DATA_STYLE 2
#define service_data_root PSTR("")
#define service_data_style PSTR("style")


#define SERVICE_AUTH_ROOT 	1
#define SERVICE_AUTH_EDIT	2
#define SERVICE_AUTH_LOGIN	3
#define SERVICE_AUTH_LOGOUT	4
#define SERVICE_AUTH_COOKIE     5
#define service_auth_root	PSTR("")
#define service_auth_edit	PSTR("edit")
#define service_auth_login	PSTR("login")
#define service_auth_logout	PSTR("logout")
#define service_auth_cookie     PSTR("cookie")


#define SERVICE_IR_ROOT 1
#define SERVICE_IR_READ 2
#define SERVICE_IR_SEND 3
#define service_ir_root PSTR("")
#define service_ir_read PSTR("read")
#define service_ir_send PSTR("send")



#endif //#ifndef _SERVICES_SERVER_H_
