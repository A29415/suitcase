#include "sys.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "lcd.h"
#include "key.h"
#include "sram.h"
#include "malloc.h"
#include "usmart.h"
#include "sdio_sdcard.h"
#include "malloc.h"
#include "w25qxx.h"
#include "ff.h"
#include "exfuns.h"
#include "fontupd.h"
#include "text.h"
#include "piclib.h"
#include "string.h"
#include "math.h"

#include "beep.h"
#include "gps.h"
#include "usart3.h"
#include "usart2.h"
#include "iwdg.h"
#include "dht11.h"
#include "string.h"
#include "rain.h"
#include "exti.h"

extern unsigned char KEY_value;

u8 USART1_TX_BUF[USART3_MAX_RECV_LEN]; 					//����1,���ͻ�����
nmea_msg gpsx; 											//GPS��Ϣ
__align(4) u8 dtbuf[50];   								//��ӡ������
const u8*fixmode_tbl[4]= {"Fail","Fail"," 2D "," 3D "};	//fix mode�ַ���
__align(4) u8 longtitude[50];
__align(4) u8 latitude[50];
__align(4) u8 altitude[50];
__align(4) u8 speed[50];
__align(4) u8 Tem[50];
__align(4) u8 Hum[50];
u8 t=0;
u8 temperature;
u8 humidity;
int rain;

void showPicture(void);
void Gps_Msg_Show(void);
extern int getrain(void);
void sendData(void);



//����GPS��λ��Ϣ
void sendData(void)
{
    float tp;
    tp=gpsx.longitude;
    //�õ������ַ���
    delay_ms(100);
    //sprintf((char *)longtitude,"Longitude:%.5f %1c   ",tp/=100000,gpsx.ewhemi);
    sprintf((char *)longtitude,"%.5f %1c   ",tp/=100000,gpsx.ewhemi);
    delay_ms(10);
    //u2_printf("%s\r\n",(char*)longtitude);
    //{ "msg": "Hello, World!" }
    u2_printf("{ \"Longitude\": \"%s\" }\r\n",(char*)longtitude);
    //printf("{ \"Longitude\": \"%s\" }\r\n",(char*)longtitude);

    tp=gpsx.latitude;
    //�õ�γ���ַ���
    delay_ms(100);
    //sprintf((char *)latitude,"Latitude:%.5f %1c   ",tp/=100000,gpsx.nshemi);
    sprintf((char *)latitude,"%.5f %1c   ",tp/=100000,gpsx.nshemi);
    delay_ms(10);
    //u2_printf("%s\r\n",(char*)latitude);
    u2_printf("{ \"Latitude\": \"%s\" }\r\n",(char*)latitude);

    tp=gpsx.altitude;
    //�õ��߶��ַ���
    delay_ms(100);
    //sprintf((char *)altitude,"Altitude:%.1fm     ",tp/=10);
    sprintf((char *)altitude,"%.1fm     ",tp/=10);
    delay_ms(10);
    //u2_printf("%s\r\n",(char*)altitude);
    u2_printf("{ \"Altitude\": \"%s\" }\r\n",(char*)altitude);

    tp=gpsx.speed;
    //�õ��ٶ��ַ���
    delay_ms(100);
    //sprintf((char *)speed,"Speed:%.3fkm/h     ",tp/=1000);
    sprintf((char *)speed,"%.3fkm/h     ",tp/=1000);
    delay_ms(10);
    //u2_printf("%s\r\n",(char*)speed);
    u2_printf("{ \"Speed\": \"%s\" }\r\n",(char*)speed);
    //-----------------dht11----------------------

    //�õ��¶��ַ���
    delay_ms(10);
    //sprintf((char *)Tem,"%d��C ",temperature);
    //delay_ms(10);
    u2_printf("{ \"Temperature\": \"%dC\" }\r\n",temperature);
    delay_ms(10);
    u2_printf("{ \"Humidity\": \"%d%%\" }\r\n",humidity);
//	LCD_ShowString(30,600,200,16,16,"send ok^_^");
   // printf("{ \"Temperature\": \"%dC\" }\r\n",temperature);

//	//�ж��Ƿ���ˮ
//	rain= getrain();
//	delay_ms(10);
//	u2_printf("{ \"Rain\": \"%d\" }\r\n",rain);
}

//��ʾGPS��λ��Ϣ
void Gps_Msg_Show(void)
{

    float tp;
    //int temp;

    POINT_COLOR=RED;
    LCD_ShowString(30,20,200,16,16,"STM32F1  M4^_^");
    LCD_ShowString(30,40,200,16,16,"Anti-lost Package");
    LCD_ShowString(30,60,200,16,16,"IoT_2518");

    POINT_COLOR=BLUE;
    //temp = temperature;
    DHT11_Read_Data(&temperature,&humidity);	//��ȡ��ʪ��ֵ
    //LCD_ShowNum(30+90,80,temperature,2,16);	//��ʾ�¶�
    //LCD_ShowNum(30+70,100,humidity,2,16);		//��ʾʪ��
    sprintf((char *)dtbuf,"Temprature: %d C",temperature);
    LCD_ShowString(30,80,200,16,16,dtbuf);	//�� ��C

    //temp=humidity;
    sprintf((char *)dtbuf,"Humidity: %d %%",humidity);
    LCD_ShowString(30,100,200,16,16,dtbuf);

    tp=gpsx.longitude;
    sprintf((char *)dtbuf,"Longitude:%.5f %1c   ",tp/=100000,gpsx.ewhemi);	//�õ������ַ���
    LCD_ShowString(30,120,200,16,16,dtbuf);
    tp=gpsx.latitude;
    sprintf((char *)dtbuf,"Latitude:%.5f %1c   ",tp/=100000,gpsx.nshemi);	//�õ�γ���ַ���
    LCD_ShowString(30,140,200,16,16,dtbuf);
    tp=gpsx.altitude;
    sprintf((char *)dtbuf,"Altitude:%.1fm     ",tp/=10);	    			//�õ��߶��ַ���
    LCD_ShowString(30,160,200,16,16,dtbuf);
    tp=gpsx.speed;
    sprintf((char *)dtbuf,"Speed:%.3fkm/h     ",tp/=1000);		    		//�õ��ٶ��ַ���
    LCD_ShowString(30,180,200,16,16,dtbuf);
    if(gpsx.fixmode<=3)														//��λ״̬
    {
        sprintf((char *)dtbuf,"Fix Mode:%s",fixmode_tbl[gpsx.fixmode]);
        LCD_ShowString(30,200,200,16,16,dtbuf);
    }
    sprintf((char *)dtbuf,"GPS+BD Valid satellite:%02d",gpsx.posslnum);	 		//���ڶ�λ��GPS������
    LCD_ShowString(30,220,200,16,16,dtbuf);
    sprintf((char *)dtbuf,"GPS Visible satellite:%02d",gpsx.svnum%100);	 		//�ɼ�GPS������
    LCD_ShowString(30,240,200,16,16,dtbuf);

    sprintf((char *)dtbuf,"BD Visible satellite:%02d",gpsx.beidou_svnum%100);	 		//�ɼ�����������
    LCD_ShowString(30,260,200,16,16,dtbuf);

    sprintf((char *)dtbuf,"UTC Date:%04d/%02d/%02d   ",gpsx.utc.year,gpsx.utc.month,gpsx.utc.date);	//��ʾUTC����
    LCD_ShowString(30,280,200,16,16,dtbuf);
    sprintf((char *)dtbuf,"CN  Time:%02d:%02d:%02d   ",gpsx.utc.hour+8,gpsx.utc.min,gpsx.utc.sec);	//��ʾUTCʱ��
    LCD_ShowString(30,300,200,16,16,dtbuf);
}


//�õ�path·����,Ŀ���ļ����ܸ���
//path:·��
//����ֵ:����Ч�ļ���
u16 pic_get_tnum(u8 *path)
{
    u8 res;
    u16 rval=0;
    DIR tdir;	 		//��ʱĿ¼
    FILINFO tfileinfo;	//��ʱ�ļ���Ϣ
    u8 *fn;
    res=f_opendir(&tdir,(const TCHAR*)path); 	//��Ŀ¼
    tfileinfo.lfsize=_MAX_LFN*2+1;				//���ļ�����󳤶�
    tfileinfo.lfname=mymalloc(SRAMIN,tfileinfo.lfsize);//Ϊ���ļ������������ڴ�
    if(res==FR_OK&&tfileinfo.lfname!=NULL)
    {
        while(1)//��ѯ�ܵ���Ч�ļ���
        {
            res=f_readdir(&tdir,&tfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
            if(res!=FR_OK||tfileinfo.fname[0]==0)break;	//������/��ĩβ��,�˳�
            fn=(u8*)(*tfileinfo.lfname?tfileinfo.lfname:tfileinfo.fname);
            res=f_typetell(fn);
            if((res&0XF0)==0X50)//ȡ����λ,�����ǲ���ͼƬ�ļ�
            {
                rval++;//��Ч�ļ�������1
            }
        }
    }
    return rval;
}


void showPicture(void)
{
    u8 res;
    DIR picdir;	 		//ͼƬĿ¼
    FILINFO picfileinfo;//�ļ���Ϣ
    u8 *fn;   			//���ļ���
    u8 *pname;			//��·�����ļ���
    u16 totpicnum; 		//ͼƬ�ļ�����
    u16 curindex;		//ͼƬ��ǰ����

    u8 t;
    u16 temp;
    u16 *picindextbl;	//ͼƬ������ ��
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
    //delay_init(168);  //��ʼ����ʱ����
    //uart_init(115200);		//��ʼ�����ڲ�����Ϊ115200
//    LED_Init();					//��ʼ��LED
//    usmart_dev.init(84);		//��ʼ��USMART
//    LCD_Init();					//LCD��ʼ��
//    KEY_Init();					//������ʼ��
    W25QXX_Init();				//��ʼ��W25Q128
    my_mem_init(SRAMIN);		//��ʼ���ڲ��ڴ��
    my_mem_init(SRAMCCM);		//��ʼ��CCM�ڴ��
    exfuns_init();			//Ϊfatfs��ر��������ڴ�
    f_mount(fs[0],"0:",1); 	//����SD��
    f_mount(fs[1],"1:",1); 	//����FLASH.
    POINT_COLOR=RED;
    while(font_init()) 		//����ֿ�
    {
        LCD_ShowString(30,50,200,16,16,"Font Error!");
        delay_ms(200);
        LCD_Fill(30,50,240,66,WHITE);//�����ʾ
        delay_ms(200);
    }

    while(f_opendir(&picdir,"0:/PICTURE"))//��ͼƬ�ļ���
    {
        Show_Str(30,170,240,16,"PICTURE�ļ��д���!",16,0);
        delay_ms(200);
        LCD_Fill(30,170,240,186,WHITE);//�����ʾ
        delay_ms(200);
    }
    totpicnum=pic_get_tnum("0:/PICTURE"); //�õ�����Ч�ļ���
    while(totpicnum==NULL)//ͼƬ�ļ�Ϊ0
    {
        Show_Str(30,170,240,16,"û��ͼƬ�ļ�!",16,0);
        delay_ms(200);
        LCD_Fill(30,170,240,186,WHITE);//�����ʾ
        delay_ms(200);
    }
    picfileinfo.lfsize=_MAX_LFN*2+1;						//���ļ�����󳤶�
    picfileinfo.lfname=mymalloc(SRAMIN,picfileinfo.lfsize);	//Ϊ���ļ������������ڴ�
    pname=mymalloc(SRAMIN,picfileinfo.lfsize);				//Ϊ��·�����ļ��������ڴ�
    picindextbl=mymalloc(SRAMIN,2*totpicnum);				//����2*totpicnum���ֽڵ��ڴ�,���ڴ��ͼƬ����
    while(picfileinfo.lfname==NULL||pname==NULL||picindextbl==NULL)//�ڴ�������
    {
        Show_Str(30,170,240,16,"�ڴ����ʧ��!",16,0);
        delay_ms(200);
        LCD_Fill(30,170,240,186,WHITE);//�����ʾ
        delay_ms(200);
    }
    //��¼����
    res=f_opendir(&picdir,"0:/PICTURE"); //��Ŀ¼
    if(res==FR_OK)
    {
        curindex=0;//��ǰ����Ϊ0
        while(1)//ȫ����ѯһ��
        {
            temp=picdir.index;								//��¼��ǰindex
            res=f_readdir(&picdir,&picfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
            if(res!=FR_OK||picfileinfo.fname[0]==0)break;	//������/��ĩβ��,�˳�
            fn=(u8*)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);
            res=f_typetell(fn);
            if((res&0XF0)==0X50)//ȡ����λ,�����ǲ���ͼƬ�ļ�
            {
                picindextbl[curindex]=temp;//��¼����
                curindex++;
            }
        }
    }
    //Show_Str(30,170,240,16,"��ʼ��ʾ...",16,0);
    delay_ms(1500);
    piclib_init();										//��ʼ����ͼ
    curindex=0;											//��0��ʼ��ʾ
    res=f_opendir(&picdir,(const TCHAR*)"0:/PICTURE"); 	//��Ŀ¼
    while(res==FR_OK)//�򿪳ɹ�
    {
        dir_sdi(&picdir,picindextbl[curindex]);			//�ı䵱ǰĿ¼����
        res=f_readdir(&picdir,&picfileinfo);       		//��ȡĿ¼�µ�һ���ļ�
        if(res!=FR_OK||picfileinfo.fname[0]==0)break;	//������/��ĩβ��,�˳�
        fn=(u8*)(*picfileinfo.lfname?picfileinfo.lfname:picfileinfo.fname);
        strcpy((char*)pname,"0:/PICTURE/");				//����·��(Ŀ¼)
        strcat((char*)pname,(const char*)fn);  			//���ļ������ں���
        LCD_Clear(BLACK);
        ai_load_picfile(pname,0,0,lcddev.width,lcddev.height,1);//��ʾͼƬ
        Show_Str(2,2,240,16,pname,16,1); 				//��ʾͼƬ����
        t=0;
        while(1)
        {
            if(KEY_value==key_0)
            {
                KEY_value = key_free;
                res=!FR_OK;
                //Gps_Msg_Show();
                break;
            }
        }
        res=0;
		res=!FR_OK; 
    }
    myfree(SRAMIN,picfileinfo.lfname);	//�ͷ��ڴ�
    myfree(SRAMIN,pname);				//�ͷ��ڴ�
    myfree(SRAMIN,picindextbl);			//�ͷ��ڴ�

    LCD_Clear(WHITE);
    //Gps_Msg_Show();
    KEY_value = key_free;
}



//---------------------------------------------------------------------------------
int main(void)
{
    u16 i,rxlen;
    u16 lenx;
    u8 key =0XFF;
//	u8 k;
//	u8 upload=0;
    delay_init(168);	    	 //��ʱ������ʼ��
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//�����ж����ȼ�����Ϊ��2��2λ��ռ���ȼ���2λ��Ӧ���ȼ�
    uart_init(115200);	 	//���ڳ�ʼ��Ϊ115200

    usmart_dev.init(72);		//��ʼ��USMART
    LED_Init();		  			//��ʼ����LED���ӵ�Ӳ���ӿ�
    KEY_Init();					//��ʼ������
    BEEP_Init();
    EXTIX_Init();
    LCD_Init();			   		//��ʼ��LCD
    usart2_init(9600);
    usart3_init(115200);		//��ʼ������3

    TIM3_Int_Init(4999,83);	//��ʱ����ʼ��(10ms�ж�),���ڸ�lvgl�ṩ10ms����������

    TIM2_Int_Init(20000-1,8400-1);		//100ms�ж�
    IWDG_Init(4,2000);    //���Ƶ��Ϊ64,����ֵΪ500,���ʱ��Ϊ1s

    POINT_COLOR=RED;
    LCD_ShowString(30,20,200,16,16,"STM32F4  M4^_^");
    LCD_ShowString(30,40,200,16,16,"Anti-lost Package");
    LCD_ShowString(30,60,200,16,16,"IoT_2518");

    //-----------dht11----------------
    while(DHT11_Init())	//DHT11��ʼ��
    {
        LCD_ShowString(30,130,200,16,16,"DHT11 Error");
        delay_ms(200);
        LCD_Fill(30,130,239,130+16,WHITE);
        delay_ms(200);
    }
//------dht11-----
    if(t%10==0)			//ÿ100ms��ȡһ��
    {
        DHT11_Read_Data(&temperature,&humidity);	//��ȡ��ʪ��ֵ
        //LCD_ShowString(30,80,200,16,16,"KEY0:Upload NMEA Data SW");
        //LCD_ShowString(30,100,200,16,16,"NMEA Data Upload:OFF");
    }
    delay_ms(10);
    t++;
    if(t==20)
    {
        t=0;
        //LED0=!LED0;
    }


    if(SkyTra_Cfg_Rate(5)!=0)	//���ö�λ��Ϣ�����ٶ�Ϊ5Hz,˳���ж�GPSģ���Ƿ���λ.
    {
        LCD_ShowString(30,120,200,16,16,"SkyTraF8-BD Setting...");
        do
        {
            usart3_init(9600);			//��ʼ������3������Ϊ9600
            SkyTra_Cfg_Prt(3);			//��������ģ��Ĳ�����Ϊ38400
            usart3_init(38400);			//��ʼ������3������Ϊ38400
            key=SkyTra_Cfg_Tp(100000);	//�������Ϊ100ms
        } while(SkyTra_Cfg_Rate(5)!=0&&key!=0);//����SkyTraF8-BD�ĸ�������Ϊ5Hz
        LCD_ShowString(30,120,200,16,16,"SkyTraF8-BD Set Done!!");
        delay_ms(500);
        LCD_Fill(30,120,30+200,120+16,WHITE);//�����ʾ
    }


    while(1)
    {
        delay_ms(1);
        if(USART3_RX_STA&0X8000)		//���յ�һ��������
        {
            rxlen=USART3_RX_STA&0X7FFF;	//�õ����ݳ���
            for(i=0; i<rxlen; i++)USART1_TX_BUF[i]=USART3_RX_BUF[i];
            USART3_RX_STA=0;		   	//������һ�ν���
            USART1_TX_BUF[i]=0;			//�Զ����ӽ�����
            GPS_Analysis(&gpsx,(u8*)USART1_TX_BUF);//�����ַ���
            Gps_Msg_Show();
            //LCD_Clear(WHITE);
            //showPicture();
            //sendData();
            //if(upload)printf("\r\n%s\r\n",USART1_TX_BUF);//���ͽ��յ������ݵ�����

        }
        //LCD_Clear(WHITE);
        //showPicture();
        //k=KEY_Scan(0);
        //if(k==KEY0_PRES)
        if(KEY_value==key_0)
        {
            KEY_value = key_free;
            LCD_Clear(BLACK);
            //LCD_ShowString(30,120,200,16,16,"Loading...");
            showPicture();

        }
        delay_ms(500);
        KEY_value = key_free;

        if((lenx%500)==0)
            LED0=!LED0;
        //IWDG_Feed();
        lenx++;

    }

}







