#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_
//#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <util/delay.h>
#include "tcpip/ip_arp_udp_tcp.h"
#include "tcpip/enc28j60.h"
#include "tcpip/net.h"
#include "ServicesServer.h"

/********** NETWORKING OPTIONS **********/
const static uint8_t my_mac[6] = {0x70,0x69,0x6c,0x6f,0x74, 0x00};
const static uint8_t my_ip[4] = {192,168,1,240};
const static uint8_t my_www_port = 80;

#define BUFFER_SIZE 1200
#define SERVICE_SIZE 4
#define PROCEDURE_SIZE 6
#define DATA_SIZE 500
#define AUTH_COOKIE_SIZE 8

#define TCP_HEADER_SIZE 0x36



/********** PUBLIC PROCEDURES **********/
void http_server_run(); 

#endif //#ifndef _HTTP_SERVER_H_
