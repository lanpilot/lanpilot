#include "ServicesServer.h"


uint8_t* services_buf;
uint16_t services_dat_p;
uint16_t services_buf_size;
uint8_t* services_service;
uint8_t services_service_num;
uint8_t* services_procedure;
uint8_t services_procedure_num;
uint8_t* services_auth_cookie;
uint8_t* services_data;
uint8_t services_url_nest_level;



void services_init()
{
	data_init();
	auth_init();
	ir_init();
}


uint8_t services_parse_input()
{
	/* If data contains a slash - '/',
	 * then this is improper situation
	 */
	if(strchr(services_data, '/') != 0){
		return 0;
	}
	services_url_nest_level = 1;
	if(strcmp_P(services_service, service_root_name)==0){
		services_service_num = SERVICE_ROOT;
		if(strcmp_P(services_procedure, service_root_root)==0){
			services_procedure_num = SERVICE_ROOT_ROOT;
			services_url_nest_level = 0;
		}else{
			return 0;
		}
		return 1;

	} else if(strcmp_P(services_service, service_data_name)==0){
		services_service_num = SERVICE_DATA;
		if(strcmp_P(services_procedure, service_data_root)==0){
			services_procedure_num = SERVICE_DATA_ROOT;
		}else if(strcmp_P(services_procedure, service_data_style)==0){
			services_procedure_num = SERVICE_DATA_STYLE;
			services_url_nest_level = 2;
		}else{
			return 0;
		}
		return 1;

        } else if(strcmp_P(services_service, service_auth_name)==0){
		services_service_num = SERVICE_AUTH;
		if(strcmp_P(services_procedure, service_auth_root)==0){
			services_procedure_num = SERVICE_AUTH_ROOT;
		}else if(strcmp_P(services_procedure, service_auth_edit)==0){
			services_procedure_num = SERVICE_AUTH_EDIT;
			services_url_nest_level = 2;
		}else if(strcmp_P(services_procedure, service_auth_login)==0){
			services_procedure_num = SERVICE_AUTH_LOGIN;
			services_url_nest_level = 2;
		}else if(strcmp_P(services_procedure, service_auth_logout)==0){
			services_procedure_num = SERVICE_AUTH_LOGOUT;
			services_url_nest_level = 2;
                }else if(strcmp_P(services_procedure, service_auth_cookie)==0){
                        services_procedure_num = SERVICE_AUTH_COOKIE;
                        services_url_nest_level = 2;
		}else{
			return 0;
		}
		return 1;
        } else if(strcmp_P(services_service, service_ir_name)==0){
		services_service_num = SERVICE_IR;
		if(strcmp_P(services_procedure, service_ir_root)==0){
			services_procedure_num = SERVICE_IR_ROOT;
		}else if(strcmp_P(services_procedure, service_ir_read)==0){
			services_procedure_num = SERVICE_IR_READ;
			services_url_nest_level = 2;
		}else if(strcmp_P(services_procedure, service_ir_send)==0){
			services_procedure_num = SERVICE_IR_SEND;
			services_url_nest_level = 2;
		}else{	
			return 0;
		}
		return 1;
        } 
	services_url_nest_level = 0;
	return 0;
}


uint8_t services_can_user_access()
{
	return auth_can_user_access(services_service_num, services_procedure_num, services_auth_cookie);
}

void services_generate_http_headers()
{
	services_dat_p += data_http_header_html(services_buf+services_dat_p, services_buf_size-services_dat_p);
}

void services_generate_http_cookie_headers(uint8_t* auth_code)
{
	services_dat_p += data_http_header_cookie(services_buf+services_dat_p, services_buf_size-services_dat_p, auth_code);
}

void services_generate_html_headers()
{
	services_generate_http_headers();
	services_dat_p += data_html_header(services_buf+services_dat_p, services_buf_size-services_dat_p, services_url_nest_level);
	services_dat_p += auth_login_bar(services_buf+services_dat_p, services_buf_size-services_dat_p, services_auth_cookie, services_url_nest_level);
}

void services_generate_not_found()
{
	//services_generate_http_headers();
	services_dat_p += data_404(services_buf+services_dat_p, services_buf_size-services_dat_p);
}

void services_generate_data_too_long()
{
	services_dat_p += data_input_too_long(services_buf+services_dat_p, services_buf_size-services_dat_p);
}

void services_generate_unauthorized()
{
	//services_generate_http_headers();
	services_dat_p += data_401(services_buf+services_dat_p, services_buf_size-services_dat_p);
}

void services_index()
{
	services_dat_p += snprintf_P(services_buf+services_dat_p,
			services_buf_size-services_dat_p, PSTR(
			"<h1>Installed services:</h1><div id=\"l\"><ul>"
			"<li><a href=\"auth/\">Administration</a>"
			"<li><a href=\"ir/\">LanPilot</a>"
			"</ul></div></body></html>"
			));
}

void services_execute_request()
{
	switch(services_service_num){
	case SERVICE_ROOT:
		switch(services_procedure_num){
		case SERVICE_ROOT_ROOT:
			services_generate_html_headers();
			services_index();
			break;
		}
		break;
	case SERVICE_DATA:
		switch(services_procedure_num){
		case SERVICE_DATA_ROOT:
			services_generate_html_headers();
			services_dat_p += data_index(services_buf+services_dat_p, services_buf_size-services_dat_p);
			break;
		case SERVICE_DATA_STYLE:
			services_generate_http_headers();
			services_dat_p += data_style(services_buf+services_dat_p, services_buf_size-services_dat_p);
			break;
		}
		break;
	case SERVICE_AUTH:
		switch(services_procedure_num){
		case SERVICE_AUTH_ROOT:
			services_generate_html_headers();
			services_dat_p += auth_index(services_buf+services_dat_p,services_buf_size-services_dat_p);
			break;
		case SERVICE_AUTH_EDIT:
			services_generate_html_headers();
			services_dat_p += auth_edit(services_buf+services_dat_p, services_buf_size-services_dat_p, services_data);
			break;
		case SERVICE_AUTH_LOGIN:
			services_generate_html_headers();
			services_dat_p += auth_login(services_buf+services_dat_p, services_buf_size-services_dat_p, services_data);
			break;
		case SERVICE_AUTH_LOGOUT:
			services_generate_html_headers();
			services_dat_p += auth_logout(services_buf+services_dat_p, services_buf_size-services_dat_p, services_auth_cookie);
			break;
		case SERVICE_AUTH_COOKIE:
			services_generate_http_cookie_headers(auth_set_cookie(services_data));
			services_dat_p += auth_set_cookie_redirect(services_buf+services_dat_p, services_buf_size-services_dat_p);
			break;
		}
		break;
	case SERVICE_IR:
		switch(services_procedure_num){
		case SERVICE_IR_ROOT:
			services_generate_html_headers();
			services_dat_p += ir_index(services_buf+services_dat_p, services_buf_size-services_dat_p);
			break;
		case SERVICE_IR_READ:
			services_generate_html_headers();
			services_dat_p += ir_read(services_buf+services_dat_p, services_buf_size-services_dat_p, services_data);
			break;
		case SERVICE_IR_SEND:
			services_generate_html_headers();
			services_dat_p += ir_send(services_buf+services_dat_p, services_buf_size-services_dat_p, services_data);
			break;
		}
		break;
	}

}


uint16_t services_process_user_request(uint8_t *_buf, uint16_t _buf_size, uint8_t *_service, uint8_t *_procedure,
				uint8_t *_auth_cookie, uint8_t *_data, uint8_t _is_data_ok)
{
	services_buf = _buf;
	services_buf_size = _buf_size;
	services_dat_p = 0;
	services_service = _service;
	services_procedure = _procedure;
	services_auth_cookie = _auth_cookie;
	services_data = _data;

	if(!_is_data_ok){
		services_generate_data_too_long();
		return services_dat_p;
	}
	if(!services_parse_input()){
		services_generate_not_found();
		return services_dat_p;
	}

	if(!services_can_user_access()){
		services_generate_unauthorized();
		return services_dat_p;
	}

	services_execute_request();

	return services_dat_p;

}
