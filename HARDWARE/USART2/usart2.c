#include "delay.h"
#include "usart2.h"
#include "stdarg.h"
#include "stdio.h"
#include "string.h"
#include "timer.h"
#include "led.h"
#include "beep.h"
#include "core_cm4.h"
//////////////////////////////////////////////////////////////////////////////////

void soft_reset(void)
{
    // �ر������ж�
    __set_FAULTMASK(1);
    // ��λ
    NVIC_SystemReset();
}

//���ڷ��ͻ�����
__align(8) u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	//���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
#ifdef USART2_RX_EN   								//���ʹ���˽���   	  
//���ڽ��ջ�����
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//���ջ���,���USART2_MAX_RECV_LEN���ֽ�.


//ͨ���жϽ�������2���ַ�֮���ʱ������100ms�������ǲ���һ������������.
//���2���ַ����ռ������100ms,����Ϊ����1����������.Ҳ���ǳ���100msû�н��յ�
//�κ�����,���ʾ�˴ν������.
//���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
vu16 USART2_RX_STA=0;
// �����жϷ�����
void USART2_IRQHandler(void)
{
    uint8_t ucTemp;
    if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)
    {
        ucTemp = (char)USART_ReceiveData(USART2);
        //LED1=!LED1;
        //����
        if(ucTemp == '0')
        {
            LED1=0;
        }
        if(ucTemp == '1')
        {
            LED1=1;
        }
        if(ucTemp == '3')
        {
            //BEEP=0;
            soft_reset();
        }
        if(ucTemp == '5')
        {
            BEEP=0;
            LED1=1;
        }
        //��beep
        if(ucTemp == '2')
        {
            //while(ucTemp == '1')
            while(1)
            {
                BEEP=1;
                delay_ms(200);
                BEEP=0;
                delay_ms(200);
                //BEEP=0;
                ucTemp = (char)USART_ReceiveData(USART2);
                if(ucTemp == '0')
                {
                    LED1=0;
                }
                if(ucTemp == '1')
                {
                    LED1=1;
                }
                if(ucTemp == '3')
                {
                    BEEP=0;
                    break;
                }
                if(ucTemp == '5')
                {
                    BEEP=0;
                    LED1=0;
                    break;
                }
            }
        }
        //ȫ��
        if(ucTemp == '4')
        {
            LED1=0;
            while(1)
            {
                BEEP=1;
                delay_ms(200);
                BEEP=0;
                delay_ms(200);
                ucTemp = (char)USART_ReceiveData(USART2);
                if(ucTemp == '0')
                {
                    LED1=0;
                }
                if(ucTemp == '1')
                {
                    LED1=1;
                }
                if(ucTemp == '3')
                {
                    BEEP=0;
                    break;
                }
                if(ucTemp == '5')
                {
                    BEEP=0;
                    LED1=1;
                    break;
                }
            }

        }


    }
}

#endif
//��ʼ��IO ����2
//bound:������
void usart2_init(u32 bound)
{

    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;


    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //ʹ��GPIOAʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//ʹ��USART2ʱ��

    USART_DeInit(USART2);  //��λ����2

    GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA11����ΪUSART2
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA10����ΪUSART2

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA11��GPIOA10��ʼ��
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//�ٶ�50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //���츴�����
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
    GPIO_Init(GPIOA,&GPIO_InitStructure); //��ʼ��GPIOA11����GPIOA10

    USART_InitStructure.USART_BaudRate = bound;//������
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
    USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ

    USART_Init(USART2, &USART_InitStructure); //��ʼ������3

    USART_Cmd(USART2, ENABLE);               //ʹ�ܴ���

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//�����ж�

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//�����ȼ�3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
    NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
//	TIM2_Int_Init(20000-1,8400-1);		//100ms�ж�
    USART2_RX_STA=0;		//����
    //TIM_Cmd(TIM7, DISABLE); //�رն�ʱ��7


}

//����2,printf ����
//ȷ��һ�η������ݲ�����USART2_MAX_SEND_LEN�ֽ�
void u2_printf(char* fmt,...)
{
    u16 i,j;
    va_list ap;
    va_start(ap,fmt);
    vsprintf((char*)USART2_TX_BUF,fmt,ap);
    va_end(ap);
    i=strlen((const char*)USART2_TX_BUF);//�˴η������ݵĳ���
    for(j=0; j<i; j++) //ѭ����������
    {
        while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);  //�ȴ��ϴδ������
        USART_SendData(USART2,(uint8_t)USART2_TX_BUF[j]); 	 //�������ݵ�����3
    }

}














