#include "AuthServer.h"

#define users_count 3 
#define services_count 2

uint8_t permissions[users_count*services_count] = 
{
 0x01 | 0x00 | 0x04, 0x01 | 0x02 | 0x00,
 0x01 | 0x02 | 0x04, 0x01 | 0x02 | 0x04,
 0x01 | 0x02 | 0x00, 0x00 | 0x02 | 0x04
};

PGM_P users_names[users_count];
PGM_P passwords[users_count];

uint8_t users_cookies[users_count][COOKIE_LENGTH+1];


/********** FUNCTIONS IMPLEMENTED IN THIS MODULE **********/
uint8_t auth_login_parse_check_before_login(uint8_t* data, uint16_t i);
uint8_t auth_login_parse_check_login(uint8_t* data, uint16_t i);
uint8_t auth_login_parse_check_before_password(uint8_t* data, uint16_t i);
uint8_t auth_login_parse_check_password(uint8_t* data, uint16_t i, PGM_P password);
void auth_set_cookie_rand(uint8_t user);
uint8_t auth_user_get_id_by_cookie(uint8_t *cookie);
uint8_t auth_can_user_read(uint8_t user, uint8_t service);
uint8_t auth_can_user_write(uint8_t user, uint8_t service);
uint8_t auth_can_user_execute(uint8_t user, uint8_t service);
void auth_user_permissions_edit_read(uint8_t user, uint8_t service, uint8_t permission);
void auth_user_permissions_edit_write(uint8_t user, uint8_t service, uint8_t permission);
void auth_user_permissions_edit_execute(uint8_t user, uint8_t service, uint8_t permission);
void auth_user_permissions_reset(uint8_t user);
uint16_t auth_user_html_list(uint8_t* buf, uint16_t maxlen);
uint8_t auth_user_get_id_by_cookie(uint8_t *cookie);
uint8_t auth_user_get_id_by_name(uint8_t *name);
uint16_t auth_user_edit_html(uint8_t* buf, uint16_t maxlen, uint8_t* data);
uint8_t auth_edit_parse_username(uint16_t i, uint8_t* data);
uint8_t auth_edit_parse_permissions(uint8_t user, uint16_t i, uint8_t* data);
uint8_t auth_edit_parse_error(uint8_t* buf, uint16_t maxlen);
uint8_t auth_edit_redirect(uint8_t* buf, uint16_t maxlen);
/********** IMPLEMENTATION **********/

void auth_init()
{
    users_names[0] = PSTR("user0");
    users_names[1] = PSTR("user1");
    users_names[2] = PSTR("user2");


    /* ******************************* */


    passwords[0] = PSTR("password"); //remember about MAX_USER_PASSWORD
    passwords[1] = PSTR("password"); //remember about MAX_USER_PASSWORD
    passwords[2] = PSTR("password"); //remember about MAX_USER_PASSWORD


    /* ******************************* */

    auth_set_cookie_rand(0);
    auth_set_cookie_rand(1);
    auth_set_cookie_rand(2);

}

uint8_t auth_can_user_access(uint8_t service_num, uint8_t procedure_num, uint8_t* auth_cookie)
{
        uint8_t user_id =auth_user_get_id_by_cookie(auth_cookie);

        switch(service_num) {
            case 1: return 1;
            case 2: return 1;
            case 3: 
                switch(procedure_num) {
                    case 1: return auth_can_user_execute(user_id, 0);
                    case 2: return auth_can_user_write(user_id, 0);
                    case 3: return 1;
                    case 4: return 1;
                    case 5: return 1;
                    default: return 0;
                }
		break;

            case 4: 
               switch(procedure_num) {
                    case 1: return auth_can_user_execute(user_id, 1);
                    case 2: return auth_can_user_read(user_id, 1);
                    case 3: return auth_can_user_write(user_id, 1);
                    default: return 0;
                }
	       break;

            default: return 0;
        }
        return 0;
}

uint16_t auth_login_bar(uint8_t* buf, uint16_t maxlen, uint8_t* auth_cookie, uint8_t url_nest_level)
{
	uint8_t user_id = auth_user_get_id_by_cookie(auth_cookie);
	if(user_id == (uint8_t)-1){
		uint16_t added = snprintf_P(buf, maxlen, PSTR("<div id=bar>Not signed in. <a href=\""));
		while(url_nest_level--)
			added += snprintf_P(buf+added, maxlen-added, PSTR("../"));
		added += snprintf_P(buf+added, maxlen-added, PSTR("auth/login\">Sign in</a><br></div>"));
		return added;
	}else{
		uint16_t added = snprintf_P(buf, maxlen, PSTR("<div id=bar>Signed in as %S. <a href=\""), users_names[user_id]);
		while(url_nest_level--)
			added += snprintf_P(buf+added, maxlen-added, PSTR("../"));
		added += snprintf_P(buf+added, maxlen-added, PSTR("auth/logout\">Sign out</a><br></div>"));
		return added;
	}
}


uint8_t auth_login_parse_check_before_login(uint8_t* data, uint16_t i) {

        if (i!=0) return -1;
        if (data[i] != '?') return -1; i++;
        if (data[i] != 'l') return -1; i++;
        if (data[i] != '=') return -1; i++;

        return i;
}

uint8_t auth_login_parse_check_login(uint8_t* data, uint16_t i) {

        if (i!=3) return -1; 
 
        uint8_t name[MAX_USER_NAME+1];

        while (((i-3)<= MAX_USER_NAME) && (data[i] != '&')) {
                name[i-3] = data[i];
                i++;
        }

        name[i-3] = '\0';
        if ((i-3)>MAX_USER_NAME) return -1;

        return auth_user_get_id_by_name(name);
}

uint8_t auth_login_parse_check_before_password(uint8_t* data, uint16_t i) {

        if (data[i] != '&') return -1; i++;
        if (data[i] != 'p') return -1; i++;
        if (data[i] != '=') return -1; i++;

        return i;
}


uint8_t auth_login_parse_check_password(uint8_t* data, uint16_t i, PGM_P password)
{

    uint16_t j=0;    

    while ((j< MAX_USER_PASSWORD) && (i < strlen((char*)data))) {
            if (pgm_read_byte(password+j) != data[i]) return 0;
            i++;
            j++;
    }
    
    if (strlen_P(password) != j)
        return 0;

    return 1;

}

uint16_t auth_login(uint8_t* buf, uint16_t maxlen, uint8_t* data)
{
                uint8_t used = snprintf_P(buf, maxlen,
                    PSTR("<form action=\"cookie/\" method=\"GET\"><br />"
                         "Login: <input type=\"text\" name=\"l\">"
                         "&nbsp;&nbsp; Password: <input type=\"password\" name=\"p\">"
                         "&nbsp;&nbsp; <input type=\"submit\" value=\"Sign in\">"
                         "</form>"));                    
                            
                return used;
}

uint16_t auth_logout(uint8_t* buf, uint16_t maxlen, uint8_t* auth_cookie)
{
        auth_set_cookie_rand(auth_user_get_id_by_cookie(auth_cookie));

        return snprintf_P(buf, maxlen, PSTR("<meta http-equiv=refresh content=\"0; url=/\">"));

}

void auth_set_cookie_rand(uint8_t user) {

     uint8_t r,i;

     for (i=0; i<COOKIE_LENGTH; i++)
     {
        r = rand();
        if (r % 2 == 0) {
             r %=10;
             r+=48;
        } else {
             r%=26;
             r+=97;
        }         

        users_cookies[user][i] = (char)r;
     }
     users_cookies[user][COOKIE_LENGTH] = '\0';



}

uint8_t* auth_set_cookie(uint8_t* data)
{
	uint16_t i;
	uint8_t user_id, password_id;
	i = 0;
	i = auth_login_parse_check_before_login(data, i);
	if (i==255)
		return 0;

	user_id = auth_login_parse_check_login(data, i);
	if (user_id==255)
		return 0;

	i += strlen_P(users_names[user_id]); 
	i = auth_login_parse_check_before_password(data, i);
	if (i==255)
		return 0;

	password_id = auth_login_parse_check_password(data, i, passwords[user_id]);
	if (password_id == 0)
		return 0;

	auth_set_cookie_rand(user_id);
	return users_cookies[user_id];
}

uint16_t auth_set_cookie_redirect(uint8_t* buf, uint16_t maxlen)
{
	return snprintf_P(buf, maxlen, PSTR("<meta http-equiv=refresh content=\"0; url=../../\">"));
}

uint8_t auth_can_user_read(uint8_t user, uint8_t service) {

    if (user == 255)  return 0;
    else return permissions[user*services_count+service] & 0x01;
}

uint8_t auth_can_user_write(uint8_t user, uint8_t service) {

    if (user == 255)  return 0;
    else return permissions[user*services_count+service] & 0x02;
}

uint8_t auth_can_user_execute(uint8_t user, uint8_t service) {

    if (user == 255)  return 0;
    else return permissions[user*services_count+service] & 0x04;
}

void auth_user_permissions_edit_read(uint8_t user, uint8_t service, uint8_t permission) {

        if(permission) 
            permissions[user*services_count+service] |=  0x01;
        else 
            permissions[user*services_count+service] &=  ~0x01;
}

void auth_user_permissions_edit_write(uint8_t user, uint8_t service, uint8_t permission) {

        if(permission) 
            permissions[user*services_count+service] |=  0x02;
        else 
            permissions[user*services_count+service] &=  ~0x02;

}

void auth_user_permissions_edit_execute(uint8_t user, uint8_t service, uint8_t permission) {

        if(permission) 
            permissions[user*services_count+service] |=  0x04;
        else 
            permissions[user*services_count+service] &=  ~0x04;

}
void auth_user_permissions_reset(uint8_t user) {
    
    auth_user_permissions_edit_read(user, 0, 0);
    auth_user_permissions_edit_write(user, 0, 0);
    auth_user_permissions_edit_execute(user, 0, 0);

    auth_user_permissions_edit_read(user, 1, 0);
    auth_user_permissions_edit_write(user, 1, 0);
    auth_user_permissions_edit_execute(user, 1, 0);
}

uint16_t auth_user_html_list(uint8_t* buf, uint16_t maxlen) 
{
   
    uint16_t used = snprintf_P(buf, maxlen, PSTR(""));
    uint8_t i,j;

    for (i=0; i<users_count; i++) {
        used += snprintf_P(buf+used, maxlen-used, PSTR("<ul><a href=\"edit/"));
        used += snprintf_P(buf+used, maxlen-used, PSTR("%S"), users_names[i]);
        used += snprintf_P(buf+used, maxlen-used, PSTR("\">"));
        used += snprintf_P(buf+used, maxlen-used, PSTR("%S"), users_names[i]);
        used += snprintf_P(buf+used, maxlen-used, PSTR("</a>"));

        for (j=0; j<services_count; j++)
        {
            used += snprintf_P(buf+used, maxlen-used, PSTR("<li>"));

            if (auth_can_user_read(i,j))
                used+= snprintf_P(buf+used, maxlen-used, PSTR("r"));
            else used+= snprintf_P(buf+used, maxlen-used, PSTR("-"));

            if (auth_can_user_write(i,j)) 
                 used+= snprintf_P(buf+used, maxlen-used, PSTR("w"));
            else used+= snprintf_P(buf+used, maxlen-used, PSTR("-"));


            if (auth_can_user_execute(i,j)) 
                 used+= snprintf_P(buf+used, maxlen-used, PSTR("x"));
            else used+= snprintf_P(buf+used, maxlen-used, PSTR("-"));

        }

        used += snprintf_P(buf+used, maxlen-used, PSTR("</ul>"));
    }

    return used;
}

uint16_t auth_index(uint8_t* buf, uint16_t maxlen)
{
 uint16_t used =  snprintf_P(buf, maxlen, PSTR(
              "<h1>AUTH Server</h1>"
              "<ul>Services<li>AUTH Server<li>IR Server</ul>"));
    used += auth_user_html_list(buf+used, maxlen-used);
    used += snprintf_P(buf+used, maxlen-used, PSTR(
              "<div></div>"
              "<br><br><br><br><br><br>"
              "<h4><a href=\"../\">Return</a></h4>"));
    return used;

}

uint8_t auth_user_get_id_by_cookie(uint8_t *cookie) {
    
    uint8_t i;

    for (i=0; i<users_count; i++) 
	    if(users_cookies[i][0] != '\0') //if there's anything at all
	        if (strncmp((char*)cookie, (char*)users_cookies[i], COOKIE_LENGTH) == 0) return i;
    return -1;
}

uint8_t auth_user_get_id_by_name(uint8_t *name) {
    
    uint8_t i;

    for (i=0; i<users_count; i++) 
        if(strcmp_P(name,users_names[i]) == 0) return i;     

    return -1;
}

uint16_t auth_user_edit_html(uint8_t* buf, uint16_t maxlen, uint8_t* data) {
    

    uint16_t used = snprintf_P(buf, maxlen, PSTR("<form method=\"GET\"><ul><li>&nbsp;r &nbsp;w &nbsp;x</li>"));
    uint8_t i,userid;

    userid = auth_user_get_id_by_name(data);

    for (i=0; i<services_count; i++)
    {
        used += snprintf_P(buf+used, maxlen-used, PSTR("<li>"));
        
        used += snprintf_P(buf+used, maxlen-used, PSTR("<input type=\"checkbox\" name=\"%d_r\""), i);
        if (auth_can_user_read(userid,i))
            used+= snprintf_P(buf+used, maxlen-used, PSTR(" checked "));
        used += snprintf_P(buf+used, maxlen-used, PSTR(">"));

        used += snprintf_P(buf+used, maxlen-used, PSTR("<input type=\"checkbox\" name=\"%d_w\""), i);
        if (auth_can_user_write(userid,i))
            used+= snprintf_P(buf+used, maxlen-used, PSTR(" checked "));
        used += snprintf_P(buf+used, maxlen-used, PSTR(">"));

        
        used += snprintf_P(buf+used, maxlen-used, PSTR("<input type=\"checkbox\" name=\"%d_x\""), i);
        if (auth_can_user_execute(userid,i))
            used+= snprintf_P(buf+used, maxlen-used, PSTR(" checked "));
        used += snprintf_P(buf+used, maxlen-used, PSTR(">"));

    }

    used+=snprintf_P(buf+used, maxlen-used, PSTR("</ul><br><br><input type=\"submit\" value=\"Save\"></form>"));

    return used;




}

uint8_t auth_edit_parse_username(uint16_t i, uint8_t* data) {

        uint8_t user_name[MAX_USER_NAME+1];

        if (i!=0) return -1;

        while ((data[i] != '?') && (i<MAX_USER_NAME)) {
            user_name[i] = data[i];
            i++;
        }
                    
        user_name[i] = '\0';

        if (data[i] != '?') return -1;

        return auth_user_get_id_by_name(user_name);

}

uint8_t auth_edit_parse_permissions(uint8_t user, uint16_t i, uint8_t* data) {

        if (data[i] != '?') return i;

        uint8_t service;
        i++;

        while (i<strlen((char*)data)) {
              if ((data[i] != '0') && (data[i] != '1'))
		      return 244;
              if (data[i] == '0')
		      service = 0;
              if (data[i] == '1')
		      service = 1;
              i++;

              if (data[i] != '_') return 245;

              i++;

              if ((data[i] != 'r') && (data[i] != 'w') && (data[i] != 'x')) return 246;

              if ((data[i+1] != '=') || (data[i+2] != 'o') || (data[i+3] != 'n')) return 247;

              switch(data[i]) {

                    case 'r': auth_user_permissions_edit_read(user, service, 1); break;
                    case 'w': auth_user_permissions_edit_write(user, service, 1); break;
                    case 'x': auth_user_permissions_edit_execute(user, service, 1); break;

              }

            i+=5;

                    
        }    

        return 1;

}

uint8_t auth_edit_parse_error(uint8_t* buf, uint16_t maxlen) {

    return snprintf_P(buf, maxlen, PSTR("Improper request."));
}

uint8_t auth_edit_redirect(uint8_t* buf, uint16_t maxlen) {

    return snprintf_P(buf, maxlen, PSTR("<meta http-equiv=refresh content=\"0; url=/auth/\">"));

}

uint16_t auth_edit(uint8_t* buf, uint16_t maxlen, uint8_t* data) {

    uint16_t i;
    uint8_t user_id;
    uint16_t used = snprintf_P(buf, maxlen, PSTR(""));

    user_id = 255;

    if (strchr(data, '?') == 0) { 

        used += snprintf_P(buf, maxlen, PSTR(
            "<h1>Edit %s's permissions</h1>"
            "<ul>Services<li>AUTH Server<li>IR Server</ul>"), data);
        used += auth_user_edit_html(buf+used, maxlen-used, data);
        used += snprintf_P(buf+used, maxlen-used, PSTR(
            "<div></div>"
            "<br><br><h4><a href=\"../../../auth/\">Return</a></h4>"
        ), data);
        
        return used;

    } else {
        i = 0;        
        user_id = auth_edit_parse_username(i, data);

        if (user_id == 255) return auth_edit_parse_error(buf+used, maxlen-used);

        auth_user_permissions_reset(user_id);

        i += strlen_P(users_names[user_id]);

        i = auth_edit_parse_permissions(user_id, i, data);

        if (i == 255) return auth_edit_parse_error(buf+used, maxlen-used);

        if (i == 1) return auth_edit_redirect(buf+used, maxlen-used);

        return auth_edit_parse_error(buf+used, maxlen-used);

    }
}

