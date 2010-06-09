#include "HTTPServer.h"

uint8_t buf[BUFFER_SIZE+1];
uint16_t dat_p; //position in buffer
uint8_t service[SERVICE_SIZE+1];
uint8_t procedure[PROCEDURE_SIZE+1];
uint8_t auth_cookie[AUTH_COOKIE_SIZE+1];
uint8_t data[DATA_SIZE+1];
uint8_t is_data_ok;


void http_blank_out_statics()
{
	memset(buf, 0, BUFFER_SIZE+1);
	memset(service, 0, SERVICE_SIZE+1);
	memset(procedure, 0, PROCEDURE_SIZE+1);
	memset(auth_cookie, 0, AUTH_COOKIE_SIZE+1);
	memset(data, 0, DATA_SIZE+1);
	is_data_ok = 1;
}

uint8_t http_match_line_beginning(PGM_P match, const uint16_t len)
{
	if(strncmp_P(buf+dat_p, match, len))
		return 0;
	
	dat_p += len;
	return 1;
}
uint8_t http_read_until_spacebar(uint8_t *dest, uint16_t maxlen)
{
	uint16_t i;
	for(i=0; i<maxlen; ++i){
		if(buf[dat_p] != ' '){
			if(buf[dat_p] != '\n')
				dest[i] = buf[dat_p];
			else
				return 0;
		}else{
			break;
		}
		dat_p++;
	}
	return 1;

}

uint8_t http_read_until_slash_or_spacebar(uint8_t *dest, uint16_t maxlen)
{
	uint16_t i;
	for(i=0; i<maxlen; ++i){
		if((buf[dat_p] != '/') && (buf[dat_p] != ' ')){
			if(buf[dat_p] != '\n')
				dest[i] = buf[dat_p];
			else
				return 0;
		}else{
			break;
		}
		dat_p++;
	}
	if(buf[dat_p] != '/')
		return 0;
	dat_p++;
	return 1;

}

void http_next_line()
{
	while((dat_p < BUFFER_SIZE) && (buf[dat_p] != '\0') && (buf[dat_p] != '\n') && (buf[dat_p] != '\r'))
		dat_p++;
	dat_p++;
	if(buf[dat_p] == '\n')
		dat_p++;
}

void http_copy_rest_of_line(uint8_t* dest, uint16_t maxlen)
{
	uint16_t i=0;
	while((i < maxlen) && (buf[dat_p] != '\0') && (buf[dat_p] != '\r')){
		dest[i] = buf[dat_p];
		i++;
		dat_p++;
	}
	http_next_line();
}



/* returns:
 * 0 - errorneus packet
 * 1 - everything OK
 */
uint8_t http_parse_packet()
{
	if(!http_match_line_beginning(PSTR("GET /"), 5))
		return 0;

	if(http_read_until_slash_or_spacebar(service, SERVICE_SIZE)){
		if(http_read_until_slash_or_spacebar(procedure, PROCEDURE_SIZE)){
			/* copying what follows /service/procedure/, we treat is as data */
			http_read_until_spacebar(data, DATA_SIZE);
			if((buf[dat_p] != '\n') && (buf[dat_p] != ' ')){ //check if data's length is OK
				is_data_ok = 0; 
			}else{
				is_data_ok = 1;
			}
		}else{
			http_next_line();
		}
	}else{
		http_next_line();
	}


	/* Now searching for line starting with "Cookie: auth=" */
	while((dat_p < BUFFER_SIZE) && (buf[dat_p] != '\0')){
		//fprintf(stderr, "bufor = %s", buf+dat_p);
		if(http_match_line_beginning(PSTR("Cookie: auth="), 13)){
			http_copy_rest_of_line(auth_cookie, AUTH_COOKIE_SIZE);
			break;
		}else{
			http_next_line();
		}
	}
	//fprintf(stderr, "Skonczylem. dat_p=%d, BUFFER_SIZE=%d; %d\n",dat_p, BUFFER_SIZE, buf[dat_p] == '\0');

	return 1;
}

void blink_led()
{
	PORTB &= ~(1<<PORTB1);
	_delay_ms(20);
	PORTB |= 1<<PORTB1;
	_delay_ms(20);

}


void http_server_run()
{
	init_ip_arp_udp_tcp(my_mac,my_ip,my_www_port);

	services_init();

	while(1){
		/* Important step. If we didn't do this, then
		 * some attacker could simply send an empty packet
		 * to re-generate the result of prievorously sent data
		 * (or do something even worse)
		 */
		http_blank_out_statics();

		rand(); // to supply true randomness

                // get the next new packet:
                uint16_t plen = enc28j60PacketReceive(BUFFER_SIZE, buf);
		

                /* plen will ne unequal to zero if there is a valid 
                 * packet (without crc error) */
                if(plen==0){
                        continue;
                }

                // arp is broadcast if unknown but a host may also
                // verify the mac address by sending it to 
                // a unicast address.
                if(eth_type_is_arp_and_my_ip(buf,plen)){
                        make_arp_answer_from_request(buf);
                        continue;
                }

                // check if ip packets are for us:
                if(eth_type_is_ip_and_my_ip(buf,plen)==0){
                        continue;
                }

                if(buf[IP_PROTO_P]==IP_PROTO_ICMP_V && buf[ICMP_TYPE_P]==ICMP_TYPE_ECHOREQUEST_V){
                        // a ping packet, let's send pong
                        make_echo_reply_from_request(buf,plen);
                        continue;
                }
		uint16_t packet_data_position;
                // tcp port www start, compare only the lower byte
                if (buf[IP_PROTO_P]==IP_PROTO_TCP_V&&buf[TCP_DST_PORT_H_P]==0&&buf[TCP_DST_PORT_L_P]==my_www_port){
                        if (buf[TCP_FLAGS_P] & TCP_FLAGS_SYN_V){
                                make_tcp_synack_from_syn(buf);
                                // make_tcp_synack_from_syn does already send the syn,ack
                                continue;
                        }
                        if (buf[TCP_FLAGS_P] & TCP_FLAGS_ACK_V){
                                init_len_info(buf); // init some data structures
                                // we can possibly have no data, just ack:
                                packet_data_position=get_tcp_data_pointer();
                                if (packet_data_position==0){
                                        if (buf[TCP_FLAGS_P] & TCP_FLAGS_FIN_V){
                                                // finack, answer with ack
                                                make_tcp_ack_from_any(buf);
                                        }
                                        // just an ack with no data, wait for next packet
                                        continue;
                                }
			}
		}
		
		make_tcp_ack_from_any(buf); // send ack for http get

		dat_p = packet_data_position;
		
		if(http_parse_packet()){
			dat_p = services_process_user_request(
					buf+packet_data_position,
					BUFFER_SIZE - packet_data_position,
					service,
					procedure,
					auth_cookie,
					data,
					is_data_ok);
			//dat_p = snprintf_P(buf+packet_data_position, BUFFER_SIZE - packet_data_position, PSTR("Witaj swiecie!"));

			make_tcp_ack_with_data(buf,dat_p); // send data

		}else{
			// Improper request
		}
	}
}
