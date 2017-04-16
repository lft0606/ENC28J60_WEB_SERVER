/**********************************************************************************
 * 文件名  ：main.c
 * 描述    ：在浏览器上创建一个web服务器，通过web里面的命令来控制开发板上的LED的亮灭。
 * 应用    ：1 在PC机的DOS界面输入： ping 192.168.1.16 ，看能否ping通(IP地址要根据自己电脑的IP来设置)。
 *           2 在IE浏览器中输入：http://192.168.1.16/123456 则会出现一个网页，通过网页
 *             中的命令可以控制开发板中的LED的亮灭。
 *          
 * 实验平台：MINI STM32F103RCT6开发板
 * 库版本  ：ST3.0.0   
**********************************************************************************/
#include "stm32f10x.h"
#include "usart1.h"
#include "led.h"
#include "spi_enc28j60.h"
#include "web_server.h"

int main (void)
{
	SystemInit();
	 
  LED_Init();	
  SPI_Enc28j60_Init();//ENC28J60初始化
		
  Web_Server();				//TCP WEB 服务程序
}

