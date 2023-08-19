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
    // 关闭所有中断
    __set_FAULTMASK(1);
    // 复位
    NVIC_SystemReset();
}

//串口发送缓存区
__align(8) u8 USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	//发送缓冲,最大USART2_MAX_SEND_LEN字节
#ifdef USART2_RX_EN   								//如果使能了接收   	  
//串口接收缓存区
u8 USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//接收缓冲,最大USART2_MAX_RECV_LEN个字节.


//通过判断接收连续2个字符之间的时间差不大于100ms来决定是不是一次连续的数据.
//如果2个字符接收间隔超过100ms,则认为不是1次连续数据.也就是超过100ms没有接收到
//任何数据,则表示此次接收完毕.
//接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
vu16 USART2_RX_STA=0;
// 串口中断服务函数
void USART2_IRQHandler(void)
{
    uint8_t ucTemp;
    if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET)
    {
        ucTemp = (char)USART_ReceiveData(USART2);
        //LED1=!LED1;
        //开灯
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
        //开beep
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
        //全开
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
//初始化IO 串口2
//bound:波特率
void usart2_init(u32 bound)
{

    NVIC_InitTypeDef NVIC_InitStructure;
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;


    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA,ENABLE); //使能GPIOA时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能USART2时钟

    USART_DeInit(USART2);  //复位串口2

    GPIO_PinAFConfig(GPIOA,GPIO_PinSource2,GPIO_AF_USART2); //GPIOA11复用为USART2
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource3,GPIO_AF_USART2); //GPIOA10复用为USART2

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //GPIOA11和GPIOA10初始化
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	//速度50MHz
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
    GPIO_Init(GPIOA,&GPIO_InitStructure); //初始化GPIOA11，和GPIOA10

    USART_InitStructure.USART_BaudRate = bound;//波特率
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
    USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
    USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式

    USART_Init(USART2, &USART_InitStructure); //初始化串口3

    USART_Cmd(USART2, ENABLE);               //使能串口

    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启中断

    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//抢占优先级2
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		//子优先级3
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器
//	TIM2_Int_Init(20000-1,8400-1);		//100ms中断
    USART2_RX_STA=0;		//清零
    //TIM_Cmd(TIM7, DISABLE); //关闭定时器7


}

//串口2,printf 函数
//确保一次发送数据不超过USART2_MAX_SEND_LEN字节
void u2_printf(char* fmt,...)
{
    u16 i,j;
    va_list ap;
    va_start(ap,fmt);
    vsprintf((char*)USART2_TX_BUF,fmt,ap);
    va_end(ap);
    i=strlen((const char*)USART2_TX_BUF);//此次发送数据的长度
    for(j=0; j<i; j++) //循环发送数据
    {
        while(USART_GetFlagStatus(USART2,USART_FLAG_TC)==RESET);  //等待上次传输完成
        USART_SendData(USART2,(uint8_t)USART2_TX_BUF[j]); 	 //发送数据到串口3
    }

}














