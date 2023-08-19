#ifndef __USART2_H
#define __USART2_H
#include "sys.h"
#include "stdio.h"


#define USART2_MAX_RECV_LEN		400					//�����ջ����ֽ���
#define USART2_MAX_SEND_LEN		400					//����ͻ����ֽ���
#define USART2_RX_EN 			1					//0,������;1,����.

extern uint8_t  USART2_RX_BUF[USART2_MAX_RECV_LEN]; 		//���ջ���,���USART2_MAX_RECV_LEN�ֽ�
extern uint8_t  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 		//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
extern __IO uint16_t USART2_RX_STA;   						//��������״̬

void usart2_init(uint32_t bound);
void u2_printf(char* fmt,...);
#endif
















