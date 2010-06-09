#include "DataServer.h"

void data_init()
{
}


uint16_t data_index(uint8_t* buf, uint16_t maxlen)
{
	return snprintf_P(buf, maxlen, PSTR("data index"));
}

uint16_t data_style(uint8_t* buf, uint16_t maxlen)
{
	return snprintf_P(buf, maxlen, PSTR(
		"body{}\n"
		"h1{text-align:center;}\n"
                "a,a:visited{color:blue}\n"
		"ul{float:left;list-style-type:none;font-family:monospace;font-weight:bold;}\n"
		"li{font-weight:normal;}\n"
                "input{margin-left:8px;}\n"
		"h4{text-align:center;}\n"
		"p{margin-top:70px}\n" // HACK: <p> works as distancer
                "#bar{text-align:right;width:100%%;font-size:0.8em}\n"));
}


uint16_t data_404(uint8_t* buf, uint16_t maxlen)
{
	return snprintf_P(buf, maxlen, PSTR("HTTP/1.0 404 Not Found\r\nContent-Type: text/html\r\n\r\n"
					"<h1>404 Not Found</h1><br>Page not found."));
}

uint16_t data_input_too_long(uint8_t* buf, uint16_t maxlen)
{
	return snprintf_P(buf, maxlen, PSTR("HTTP/1.0 414 Request-URI Too Long\r\nContent-Type: text/html\r\n\r\n"
					"<h1>414 Request-URI Too Long</h1><br>The requested URI is too long to handle."));
}

uint16_t data_401(uint8_t* buf, uint16_t maxlen)
{
	return snprintf_P(buf, maxlen, PSTR("HTTP/1.0 401 Unauthorized\r\nContent-Type: text/html\r\n\r\n"
						"<h1>401 Unauthorized</h1>Insufficient privilages."));
}

uint16_t data_http_header_html(uint8_t* buf, uint16_t maxlen)
{
	return snprintf_P(buf, maxlen, PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n"));
}

uint16_t data_http_header_cookie(uint8_t* buf, uint16_t maxlen, uint8_t* auth_code)
{
	if(auth_code == 0) {
		return data_http_header_html(buf, maxlen);
	}else{
		return snprintf_P(buf, maxlen, PSTR("HTTP/1.0 200 OK\r\nSet-Cookie: auth=%s; path=/;\r\nContent-Type: text/html\r\n\r\n"), auth_code);
	}

}

uint16_t data_html_header(uint8_t* buf, uint16_t maxlen, uint8_t url_nest_level)
{
	uint16_t added = snprintf_P(buf, maxlen, PSTR("<link rel=stylesheet href=\""));
	while(url_nest_level--)
		added += snprintf_P(buf+added, maxlen-added, PSTR("../"));
	added += snprintf_P(buf+added, maxlen-added, PSTR("data/style\"><body bgcolor=lightblue> "));
	return added;
}

