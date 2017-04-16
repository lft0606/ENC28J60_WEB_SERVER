#ifndef _TCPIP_H
#define _TCPIP_H

#define BUFFER_SIZE 1500

#define PSTR(s) s	

unsigned char verify_password(char *str);
unsigned char analyse_get_url(char *str);
unsigned int print_webpage(unsigned char *buf,unsigned char on_off);
int Web_Server(void);
#endif


