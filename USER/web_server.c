#include "enc28j60.h"
#include "ip_arp_udp_tcp.h"
#include "net.h"
#include "web_server.h"
#include <string.h>
#include "led.h"

static unsigned char mymac[6] = {0x54,0x55,0x58,0x10,0x00,0x24};//mac��ַ
static unsigned char myip[4] = {192,168,1,16};//ip��ַ
static char baseurl[]="http://192.168.1.16/";//ip��ַ(������DNS�����֣������DNS�������Ļ�)��ip��ַ������"/"��β */
static unsigned int mywwwport =80; 			//tcp/www�����˿ںţ���ΧΪ:1-254
static unsigned int myudpport =1200; 		//udp �����˿ںţ�������(������)�˿ں�
static unsigned char buf[BUFFER_SIZE+1];//�������ݻ�����
static char password[]="123456"; //����,���ܴ���9���ַ�(ֻ�������ǰ5λ�ᱻ���)��(�ַ��޶�Ϊ��a-z,0-9)

/***************************************************************
 * ��������verify_password
 * ����  ��ȷ������
 * ����  ��str
 * ���  ����
 * ����  ��
***************************************************************/ 
unsigned char verify_password(char *str)
{
    // the first characters of the received string are
    // a simple password/cookie:
    if (strncmp(password,str,5)==0)
    {
        return(1);
    }
    return(0);
}

/***************************************************************************
 * ��������analyse_get_url
 * ����  ��takes a string of the form password/commandNumber and analyse it
 * ����  ��str
 * ���  ��-1 invalid password, otherwise command number
 *         -2	no command given but password valid
 * ����  ��
***************************************************************************/
unsigned char analyse_get_url(char *str)
{
    unsigned char i=0;
    if (verify_password(str)==0)
    {
        return(-1);
    }
    // find first "/"
    // passw not longer than 9 char:
    while(*str && i<10 && *str >',' && *str<'{')
    {
        if (*str=='/')
        {
            str++;
            break;
        }
        i++;
        str++;
    }
    if (*str < 0x3a && *str > 0x2f)
    {
        // is a ASCII number, return it
        return(*str-0x30);
    }
    return(-2);
}

/********************************************************************
 * ��������print_webpage
 * ����  ��������д��tcp�ķ��ͻ�����(��ʵ��һ����ҳ)
 * ����  ��-buf
 *         -on_off
 * ���  ����
 * ����  ��-plen
*********************************************************************/
unsigned int print_webpage(unsigned char *buf,unsigned char on_off)
{
    unsigned int plen;
		/* �½�һ����ҳ�������½�һ���ļ�һ�� */
    plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\nPragma: no-cache\r\n\r\n"));
	
		/* ����ҳ��������д��Ϣ */
		plen=fill_tcp_data_p(buf,plen,PSTR("<center><p><font color=\"#000099\">����һ�� web ���Գ���,TEST BY LFT 20170415</font>"));
    plen=fill_tcp_data_p(buf,plen,PSTR("<center><p> LED1״ָ̬ʾ: "));

    if (on_off)
    {
        plen=fill_tcp_data_p(buf,plen,PSTR("<font color=\"#00FF00\"> ��</font>"));
    }
    else
    {
        plen=fill_tcp_data_p(buf,plen,PSTR("��"));
    }

    plen=fill_tcp_data_p(buf,plen,PSTR(" <small><a href=\""));
    plen=fill_tcp_data(buf,plen,baseurl);		 // Ҫ����������������ַ
    plen=fill_tcp_data(buf,plen,password);	 // ��������ַ���������
    plen=fill_tcp_data_p(buf,plen,PSTR("\">[ˢ��]</a></small></p>\n<p><a href=\""));
    // the url looks like this http://baseurl/password/command
    plen=fill_tcp_data(buf,plen,baseurl);
    plen=fill_tcp_data(buf,plen,password);
    if (on_off)
    {
        plen=fill_tcp_data_p(buf,plen,PSTR("/0\">�ر�LED1</a><p>"));
    }
    else
    {
        plen=fill_tcp_data_p(buf,plen,PSTR("/1\">����LED1</a><p>"));
    }
   
    plen=fill_tcp_data_p(buf,plen,PSTR("<font color=\"#000099\">****************MINI STM32F103RCT6������WEB SERVERʵ��****************</font>\n"));
    return(plen);
}

/**************************************************************************************
 * ��������Web_Server
 * ����  ����������ϴ���һ��web��������ͨ��web��������������ƿ������ϵ�LED������
 * ����  ����
 * ���  ����
 * ����  ��-0 ���гɹ�
 * Ӧ��  ��1 ��PC����DOS�������룺 ping 192.168.1.16 (���ܷ�pingͨ)
 *         2 ��IE����������룺http://192.168.1.16/123456 ������һ����ҳ��ͨ����ҳ
 *           �е�������Կ��ƿ������е�LED������
 **************************************************************************************/
int Web_Server(void)
{   
	unsigned int plen, i1 = 0;
	unsigned int dat_p;
	unsigned char i = 0;
	unsigned char cmd, *buf1;
	unsigned int payloadlen = 0;
    
  enc28j60Init(mymac);//��ʼ�� enc28j60 ��MAC��ַ(�����ַ),�����������Ҫ����һ��
  enc28j60PhyWrite(PHLCON,0x476);//PHY LED ����,LED����ָʾͨ�ŵ�״̬	  
	init_ip_arp_udp_tcp(mymac,myip,mywwwport);//��ʼ����̫�� IP ��

  while(1)
  {   
					
      plen = enc28j60PacketReceive(BUFFER_SIZE, buf);  // get the next new packet:	    
      
      // plen will ne unequal to zero if there is a valid packet (without crc error)			
      if(plen==0)
      {
          continue;
      }

      // arp is broadcast if unknown but a host may also
      // verify the mac address by sending it to 
      // a unicast address.		     
      if(eth_type_is_arp_and_my_ip(buf,plen))
      {
          make_arp_answer_from_request(buf);          
          continue;
      }
      
      // check if ip packets are for us:			
      if(eth_type_is_ip_and_my_ip(buf,plen)==0) 
      {
          continue;
      }      
      
      if(buf[IP_PROTO_P]==IP_PROTO_ICMP_V && buf[ICMP_TYPE_P]==ICMP_TYPE_ECHOREQUEST_V)
      {
          // a ping packet, let's send pong  DOS �µ� ping �����		 
          make_echo_reply_from_request(buf, plen);          
          continue;
      }

/*-----------------tcp port www start, compare only the lower byte-----------------------------------*/
      if (buf[IP_PROTO_P]==IP_PROTO_TCP_V&&buf[TCP_DST_PORT_H_P]==0&&buf[TCP_DST_PORT_L_P]==mywwwport)
      {
          if (buf[TCP_FLAGS_P] & TCP_FLAGS_SYN_V)
          {
              make_tcp_synack_from_syn(buf);
              // make_tcp_synack_from_syn does already send the syn,ack
              continue;
          }
          if (buf[TCP_FLAGS_P] & TCP_FLAGS_ACK_V)
          {
              init_len_info(buf); // init some data structures
              // we can possibly have no data, just ack:
              dat_p=get_tcp_data_pointer();
              if (dat_p==0)
              {
                  if (buf[TCP_FLAGS_P] & TCP_FLAGS_FIN_V)
                  {
                      // finack, answer with ack
                      make_tcp_ack_from_any(buf);
                  }
                  // just an ack with no data, wait for next packet
                  continue;
              }
              if (strncmp("GET ",(char *)&(buf[dat_p]),4)!=0)
              {
                  // head, post and other methods:
                  //
                  // for possible status codes see:
                  // http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
                  plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n<h1>200 OK</h1>"));
                  goto SENDTCP;
              }
              if (strncmp("/ ",(char *)&(buf[dat_p+4]),2)==0)
              {
                  plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n"));
                  plen=fill_tcp_data_p(buf,plen,PSTR("<p>Usage: "));
                  plen=fill_tcp_data(buf,plen,baseurl);
                  plen=fill_tcp_data_p(buf,plen,PSTR("password</p>"));
                  goto SENDTCP;
              }
              cmd=analyse_get_url((char *)&(buf[dat_p+5]));
              // for possible status codes see:
              // http://www.w3.org/Protocols/rfc2616/rfc2616-sec10.html
              if (cmd==-1)
              {
                  plen=fill_tcp_data_p(buf,0,PSTR("HTTP/1.0 401 Unauthorized\r\nContent-Type: text/html\r\n\r\n<h1>401 Unauthorized</h1>"));
                  goto SENDTCP;
              }
              if (cmd==1)	           // �û�����
              {			
									LED1(ON);
                  i=1;							 // ���� = 1
              }
              if (cmd==0)						 // �û�����
              {									
									LED1(OFF);
                  i=0;							 // ���� = 0
              }
              // if (cmd==-2) or any other value
              // just display the status:
              plen=print_webpage(buf,(i));
              
              SENDTCP:
              make_tcp_ack_from_any(buf);       // send ack for http get
              make_tcp_ack_with_data(buf,plen); // send data
              continue;
          }
      }
/*-------------------------------------- tcp port www end ---------------------------------------*/

/*--------------------- udp start, we listen on udp port 1200=0x4B0 -----------------------------*/
      if (buf[IP_PROTO_P]==IP_PROTO_UDP_V&&buf[UDP_DST_PORT_H_P]==4&&buf[UDP_DST_PORT_L_P]==0xb0)
      {
          payloadlen=	  buf[UDP_LEN_H_P];
          payloadlen=payloadlen<<8;
          payloadlen=(payloadlen+buf[UDP_LEN_L_P])-UDP_HEADER_LEN;
         
          
          ANSWER:
          for(i1=0; i1<payloadlen; i1++) 
					{
						buf1[i1]=buf[UDP_DATA_P+i1];
					}
          make_udp_reply_from_request(buf,buf1,payloadlen,myudpport);          
      }
  }
          return (0);
}
