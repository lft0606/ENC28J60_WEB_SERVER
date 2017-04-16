/************************************************************
Copyright (C), 1988-1999, Tech. Co., Ltd.
FileName:   led.c
Author  : 	LFT
Version : 	V1.0
Date    :		20170416
Description: led初始化
History: // 历史修改记录
<author> <time> <desc>
1、LFT 2017/04/16 创建该文件
2、LFT 2017/04/17 修改LED的IO
***********************************************************/
#include "led.h"

void LED_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE);

	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_2;	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOD, &GPIO_InitStructure);

	GPIO_SetBits(GPIOA, GPIO_Pin_8);
	GPIO_SetBits(GPIOD, GPIO_Pin_2);	
}
