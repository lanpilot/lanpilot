#include "IRServer.h"

void ir_init()
{
	generator_initialize();
}

uint16_t ir_index(uint8_t* buf, uint16_t maxlen)
{
	return snprintf_P(buf, maxlen, PSTR(
					"<h1>IR Server</h1>"
					"<div id=\"l\"><ul>"
					"<li><a href=\"send/\">Send a code</a>"
					"<li><a href=\"read/\">Read a code</a>"
					"</ul></div>"
					"<br><br><br><br><br><br>"
					"<h4><a href=\"../\">Return</a></h4>"
				));
}

/* Data format:
 * [frequency]_[bit_0_length]_[bit_1_length]_[bit_2_length]_[bit_3_length]_ ... [bit_n-th_length]
 *
 * frequency		 - in kilohertz
 * length of each bit	 - in units of 1/frequency
 * 
 * Bits with even numbers are ones (carrier is produced),
 * bits with odd numbers are zeros (no carrier is produced).
 * All numbers must be <= 2^16 ( = 65536), since they are
 * stored as uint16_t numbers, and > 0, since 0 is an error code.
 */



uint16_t ir_read_universal_from_PINC(uint8_t* buf, uint16_t maxlen, uint8_t* data, uint8_t pin, uint8_t frequency)
{
	generator_set_freq(frequency);
	generator_start();
#define TIMEOUT_VALUE 65000
	/* A hack to save memory */
	const uint16_t output_num_len = DATA_SIZE/2;
	uint16_t* output_num = (uint16_t*)data;
	/* ***** */
	uint8_t timeout = 0;
	uint16_t buf_position = 0;
	uint16_t position_num = 0;
	generator_tics = 0;
	while(bit_is_set(PINC, pin))
		if(generator_tics >= TIMEOUT_VALUE){
			timeout = 1;
			break;
		}
	if(!timeout){
		while(position_num < output_num_len){
			generator_tics = 0;
			if(bit_is_set(PINC, pin)){
				PORTB &= ~(1 << PB1);
				while(bit_is_set(PINC, pin))
					if(generator_tics >= TIMEOUT_VALUE){
						timeout = 1;
						break;
				}
				if(timeout)
					break;
				output_num[position_num] = generator_tics;
				position_num++;
			}else{
				PORTB |= 1 << PB1;
				while(bit_is_clear(PINC, pin))
					if(generator_tics >= TIMEOUT_VALUE){
						timeout = 1;
						break;
					}
				if(timeout)
					break;
				output_num[position_num] = generator_tics;
				position_num++;
			}
		}
	}
	generator_stop(); //we will not need it anymore
	if(timeout){
		buf_position += snprintf_P(buf+buf_position, maxlen-buf_position, PSTR("TIMEOUT"));
	}else{
		uint16_t i;
		uint16_t to_add;
		for(i=0; i<position_num; ++i){
			to_add = snprintf_P((char*)buf+buf_position, maxlen-buf_position, PSTR("%u_"), output_num[i]);
			if(to_add + buf_position >= maxlen)
				break;
			else
				buf_position += to_add;
		}
		if(position_num > 0) {
			buf[buf_position] = '\0'; //clearing the last underscore
			buf_position--;
		}
	}
	PORTB &= ~(1 << PB1); //turn LED on
	return buf_position;
}

uint16_t ir_read_universal_from_PINC_with_HTML(uint8_t* buf, uint16_t maxlen, uint8_t* data, uint8_t pin, uint8_t frequency)
{
	uint16_t buf_position = 0;
	buf_position += snprintf_P(buf+buf_position, maxlen-buf_position, PSTR(
				"<p>" // a hack, <p> works as a distancer
				"<center><textarea rows=6 cols=120 onFocus=\"this.select();\">%d_"),
				frequency
			);
	
	buf_position += ir_read_universal_from_PINC(buf+buf_position, maxlen-buf_position, data, pin, frequency);

	// WARNING !
	// the <textarea> tag is not closed here!
	// <p> is not closed here!
	return buf_position;
}


uint16_t ir_read_38(uint8_t* buf, uint16_t maxlen, uint8_t* data)
{
	return ir_read_universal_from_PINC_with_HTML(buf, maxlen, data, 1 << PC1, 38);
}

uint16_t ir_read_56(uint8_t* buf, uint16_t maxlen, uint8_t* data)
{
	return ir_read_universal_from_PINC_with_HTML(buf, maxlen, data, 1 << PC0, 56);
}

uint16_t ir_read(uint8_t* buf, uint16_t maxlen, uint8_t* data)
{
	if(strlen(data) == 0){
		// "Index" action
		return snprintf_P(buf, maxlen, PSTR(
					"<h1>Choose frequency:</h1>"
					"<div id=\"l\"><ul>"
					"<li><a href=\"38\">38 kHz</a>"
					"<li><a href=\"56\">56 kHz</a>"
					"</ul></div>"
					"<br>"
					"<br>"
					"<br>"
					"<br>"
					"Instructions:<br>"
					"Press and hold a button on remote control, then click on desired<br>"
					"frequency above. The output format is described <a href=\"../send/\">here</a>."
					"<h4><a href=\"../\">Return</a></h4>"
				));
		
	}else if(strcmp_P(data, PSTR("38")) == 0 ){
		return ir_read_38(buf, maxlen, data);
	}else if(strcmp_P(data, PSTR("56")) == 0 ){
		return ir_read_56(buf, maxlen, data);
	}else{
		//error
		return snprintf_P(buf, maxlen, PSTR(
					"<h1>Improper request</h1>"
					"<br><br><br><br><br><br>"
					"<h4><a href=\"../\">Return</a></h4>"
				));
		return 0;
	}

}


uint16_t ir_parse_number_until_underscore(uint8_t* data, uint16_t* position)
{
	uint16_t out = 0;
	while((data[*position] != '_') && (data[*position] != '\0')){
		if(!isdigit(data[*position]))
			return 0;
		out *= 10;
		out += data[*position] - '0';
		(*position)++;
	}
	return out;
}



uint16_t ir_send_improper_input(uint8_t* buf, uint16_t maxlen)
{
	return snprintf_P(buf, maxlen, PSTR(
				"<h1>Improper input.</h1><br>"
				"<br>"
				"Expected format:"
				"<pre>http://[HOST]/ir/send/[frequency]_[bit_0_length]_[bit_1_length]_ ... [bit_n-th_length]</pre>"
				"where:"
				"<div id=\"l\"><ul>"
				"<li>frequency"
				"<li>length of each bit"
				"</ul><ul>"
				"<li>- in kilohertz"
				"<li>- in units of 1/frequency"
				"</ul></div>"
				"<div></div><br>"

				"<br>"
				"<br>"
				"<br>"
				"Bits with even numbers are ones (carrier is produced),<br>"
				"bits with odd numbers are zeros (no carrier is produced).<br>"
				"All numbers must be <= 2^16 ( = 65536), since they are<br>"
				"stored as uint16_t numbers, and > 0, since 0 is an error code.<br>"
				"<br>"
				"<br>"
				"<h4><a href=\"../\">Return</a></h4>"));
}

uint16_t ir_send(uint8_t* buf, uint16_t maxlen, uint8_t* data)
{
	uint16_t data_pos = 0;
	uint16_t freq = ir_parse_number_until_underscore(data, &data_pos);
	if(freq == 0)
		return ir_send_improper_input(buf,maxlen);
		
	/* We will read numbers describing bits length
	 * and save them to buf in order not to declare
	 * additional array.
	 */
	uint16_t *bits_lengths = (uint16_t*)buf;
	uint16_t bits_lengths_size = maxlen / 2;
	uint16_t bits_lengths_read = 0;
	while(bits_lengths_read <= bits_lengths_size){
		if(data[data_pos] == '\0')
			break;
		if(data[data_pos] == '_')
			data_pos++;
		bits_lengths[bits_lengths_read] = ir_parse_number_until_underscore(data, &data_pos);
		if(bits_lengths[bits_lengths_read] == 0)
			return ir_send_improper_input(buf,maxlen);

		bits_lengths_read ++;
	}

	generator_emit_signal(freq, bits_lengths, bits_lengths_read);
	

	return snprintf_P(buf, maxlen, PSTR(
				"<h1>Sent successfully</h1><br>"
				"<br>"
				"<br>"
				"<br>"
				"<br>"
				"<br>"
				"<h4><a href=\"../\">Return</a></h4>"), freq, bits_lengths_read, generator_tics);
}


