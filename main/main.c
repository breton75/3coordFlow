#include "main.h"
#include "exam_spi.h"

#include "exam_gena.h"
#include "init_periphery.h"

#include <stdio.h>
#include <stdarg.h>


#define LED1 GPIO_Pin_7
#define LED2 GPIO_Pin_13
#define LED3 GPIO_Pin_3
#define LED4 GPIO_Pin_4
#define TIMER 5000000

//#define POPLAVOK


#include "stm32f10x_conf.h"
#include ".\..\FatFs\source\ff.h"
#include <stdbool.h>

#ifndef POPLAVOK

#include "..\RS_485\2w_485.h"
#include "..\I2C_Keller\i2c_keller.h"
#include "..\I2C_Orient\i2c_orient.h"
#include "tdc.h"

//#include "..\TDC_SPI\TI_TDC720x.h"
//#include "..\TDC_SPI\TI_TDC1000.h"
//#include "..\TDC_SPI\TI_TDC1000_register_settings.h"


#define speed_spline 16 
#define dis3 0.1056
#define dis2 0.0859
#define dis1 0.0659
#endif

//#include "..\_out_protocols\out_protocols.h"


//void I2C_burst_read(I2C_TypeDef* I2Cx, uint8_t HW_address, uint8_t addr, uint8_t n_data, uint8_t *data);
//void I2C_single_write(uint8_t HW_address, uint8_t addr);
int the_printf(const char *str, ...);
int Debag_printf_uart1(char *str);
void Send_data_to_nav(char *str,int size_to_send);
void Send_data_from_nav(char *str,int size_to_send);

typedef struct
{
  bool valid_data;
  float Longitude;
  float Latitude;
  float Speed;
  float Cource;
  float Number_of_Satellites;
  float HDOP;
  float Altitude;
  char DD;
  char MM;
  short YY;
  char hh;
  char mm;
  char ss;
  float Distance;
} gps_struct;

void init_SD_fat(FATFS*,FIL*);

//FIL		global_file;
//FATFS global_FATFS_Obj;
gps_struct gps_data;


// GPIO_SetBits(GPIOD, GPIO_Pin_14);
int total_write_navi_byte;
int file_index;
int nFiles = 0;	
UINT nRead, nWritten;
int reinit_sd_count = 0;


char	str_buff[128];		// буфер для вывода
int *sdf;


#define max_iq_b_size 512
#define max_file_size 1024*16
#define re_init_sd_after_counts 128
char* buff_1;		//буфе р для навигации, для переброса из прерывания на диск
char*	buff_2;		//буфе р для навигации, для переброса из прерывания на диск
int  buff_1_level;
int  buff_2_level;
char*	buff_work;
int* buff_work_level;

char in_array_1[256];
char in_array_2[256];
int in_array_1_level=0;
int in_array_2_level=0;
int in_array_level=0;
char*	in_array_work;
int* in_array_work_level;

gps_struct work_gps_str;




/*
char out_array_1[256];
char out_array_2[256];
int out_array_1_level;
int out_array_2_level;
char*	out_array_work;
int* out_array_work_level;
*/

int print_to_file(FIL* fp, const TCHAR* str, ...)
{
	char info_string[512];
	UINT sb=0;
	UINT bw;
	char* str_for_write=info_string;
    va_list ap;
    va_start(ap,str); // начало параметров
    vsprintf(info_string,str,ap);
    va_end(ap);     // очистка параметров

    while(*str_for_write != '\0')
    {
        sb++;
        f_write(fp,str_for_write++,1,&bw);
    }
		f_sync(fp);
		return sb;
}

int open_new_file(FIL *s_st)
{
    int result;
		FIL new_file;
    if (s_st!=0)
    {
        f_sync(s_st);
        f_close(s_st);
    }
    result=sprintf(str_buff,"nav_data _%8d.txt",file_index);
    file_index++;

    the_printf("Look for free file name\n");

    //s_st=&global_file;

    result = f_open(&new_file, str_buff, FA_CREATE_NEW | FA_WRITE);
    while (result == FR_EXIST && file_index>0)
    {
        f_close(s_st);
        the_printf("file %s already exist\n",str_buff);
        result=sprintf(str_buff,"nav_data _%8d.txt",file_index);
        file_index++;
        result = f_open(s_st, str_buff, FA_CREATE_NEW | FA_WRITE);
        the_printf("try begin %s file\n",str_buff);
    }
    if (result == FR_OK)
    {
        the_printf("begin new file : %s\n\r",str_buff);
				total_write_navi_byte=print_to_file(s_st,"Longitude,\tLatitude,\tSpeed (kilometer),\tCource (degrees),\t Number of Satellites,\t HDOP,\t Altitude (meters),\t DD/MM/YY,\t HH:MM:SS,\t Distance(meters)\n"); 
				*s_st=new_file;
			return 0;
    }
    the_printf("Error to open new file, error %d\n",result);
    s_st=0;
    return -3;
}

int char_to_int(char *cc)
{
    int out;
    if (*cc>=48 && *cc<=57)
    {
        out=(*cc)-48;
        return *cc-48;
    }
    return -1;
}

bool found_gprms(char*	gprmc_buff, int buff_size,
                 float* N,float* E,float* V, float* K, short* DD,short* MM,short* YY,short* hh,short* mm,short* ss)
{
    int poz;
    char* buf_poz;
    float minuts,koef;
    for (poz=0;poz<buff_size-40;poz++)
    {
        //если нашли заголовок
        if (gprmc_buff[poz]=='$' &&
                gprmc_buff[poz+1]=='G' &&
                gprmc_buff[poz+2]=='P' &&
                gprmc_buff[poz+3]=='R' &&
                gprmc_buff[poz+4]=='M' &&
                gprmc_buff[poz+5]=='C')
        {
            buf_poz=&gprmc_buff[poz+6];
            if (*(buf_poz++)!=',') break;
            // hhmmss
            if (*(buf_poz)==0xff) return false;
            
            *hh=char_to_int(buf_poz++)*10;
            if (*(buf_poz)==0xff) return false;
            *hh+=char_to_int(buf_poz++);
            if (*(buf_poz)==0xff) return false;

            *mm=char_to_int(buf_poz++)*10;
            if (*(buf_poz)==0xff) return false;
            *mm+=char_to_int(buf_poz++);
            if (*(buf_poz)==0xff) return false;

            *ss=char_to_int(buf_poz++)*10;
            if (*(buf_poz)==0xff) return false;
            *ss+=char_to_int(buf_poz++);
            if (*(buf_poz)==0xff) return false;

            buf_poz+=4;

            if (*(buf_poz++)!=',') break;
            if (*(buf_poz)==0xff) return false;
            if (*(buf_poz++)!='A') break;
            if (*(buf_poz)==0xff) return false;
            if (*(buf_poz++)!=',') break;
            if (*(buf_poz)==0xff) return false;


            //4807.038
            *N=char_to_int(buf_poz++)*10;
            if (*(buf_poz)==0xff) return false;
            *N+=char_to_int(buf_poz++);
            if (*(buf_poz)==0xff) return false;
            minuts=char_to_int(buf_poz++)*10;
            if (*(buf_poz)==0xff) return false;
            minuts+=char_to_int(buf_poz++);
            if (*(buf_poz)==0xff) return false;
            if (*(buf_poz++)!='.') break;
            if (*(buf_poz)==0xff) return false;
            koef=0.1;
            while (*(buf_poz)!=',')
            {
                minuts+=char_to_int(buf_poz++)*koef;
                if (*(buf_poz)==0xff) return false;
                koef/=10;
            }
            *N+=minuts/60.;
            if (*(buf_poz++)!=',') break;
            if (*(buf_poz)==0xff) return false;
            if (*(buf_poz++)=='S') *N=-(*N);
            if (*(buf_poz)==0xff) return false;
            if (*(buf_poz++)!=',') break;
            if (*(buf_poz)==0xff) return false;


            *E=char_to_int(buf_poz++)*100;
            if (*(buf_poz)==0xff) return false;
            *E+=char_to_int(buf_poz++)*10;
            if (*(buf_poz)==0xff) return false;
            *E+=char_to_int(buf_poz++);
            if (*(buf_poz)==0xff) return false;
            minuts=char_to_int(buf_poz++)*10;
            if (*(buf_poz)==0xff) return false;
            minuts+=char_to_int(buf_poz++);
            if (*(buf_poz)==0xff) return false;
            if (*(buf_poz++)!='.') break;
            if (*(buf_poz)==0xff) return false;
            koef=0.1;
            while (*(buf_poz)!=',')
            {
                minuts+=char_to_int(buf_poz++)*koef;
                if (*(buf_poz)==0xff) return false;
                koef/=10;
            }
            *E+=minuts/60.;
            if (*(buf_poz++)!=',') break;
            if (*(buf_poz)==0xff) return false;
            if (*(buf_poz++)=='W') *E=-(*E);
            if (*(buf_poz)==0xff) return false;
            if (*(buf_poz++)!=',') break;
            if (*(buf_poz)==0xff) return false;

            minuts=0;
            while (*(buf_poz)!='.')
            {
                minuts=minuts*10+char_to_int(buf_poz++);
                if (*(buf_poz)==0xff) return false;
            }
            koef=0.1;
            buf_poz++;
            while (*(buf_poz)!=',')
            {
                minuts+=char_to_int(buf_poz++)*koef;
                if (*(buf_poz)==0xff) return false;
                koef/=10;
            }
            *V=minuts;

            buf_poz++;
            minuts=0;
            while (*(buf_poz)!='.')
            {
                minuts=minuts*10+char_to_int(buf_poz++);
                if (*(buf_poz)==0xff) return false;
            }
            koef=0.1;
            buf_poz++;
            while (*(buf_poz)!=',')
            {
                minuts+=char_to_int(buf_poz++)*koef;
                if (*(buf_poz)==0xff) return false;
                koef/=10;
            }
            *K=minuts;
            buf_poz++;


            *DD=char_to_int(buf_poz++)*10;
            if (*(buf_poz)==0xff) return false;
            *DD+=char_to_int(buf_poz++);
            if (*(buf_poz)==0xff) return false;

            *MM=char_to_int(buf_poz++)*10;
            if (*(buf_poz)==0xff) return false;
            *MM+=char_to_int(buf_poz++);
            if (*(buf_poz)==0xff) return false;

            *YY=char_to_int(buf_poz++)*10;
            if (*(buf_poz)==0xff) return false;
            *YY+=char_to_int(buf_poz++);
            if (*(buf_poz)==0xff) return false;

            return true;



        }
    }
    return false;
}

bool found_gpgga(char*	gprmc_buff, int buff_size,
                 float* N,float* E,float* V, float* K, short* DD,short* MM,short* YY,short* hh,short* mm,short* ss)
{
    int poz;
    char* buf_poz;
    float minuts,koef;
    for (poz=0;poz<buff_size-40;poz++)
    {
        //если нашли заголовок
        if (gprmc_buff[poz]=='$' &&
                gprmc_buff[poz+1]=='G' &&
                gprmc_buff[poz+2]=='P' &&
                gprmc_buff[poz+3]=='G' &&
                gprmc_buff[poz+4]=='G' &&
                gprmc_buff[poz+5]=='A')
        {
            buf_poz=&gprmc_buff[poz+6];
            if (*(buf_poz++)!=',') break;
            // hhmmss
            if (*(buf_poz)==0xff) return false;
            
            *hh=char_to_int(buf_poz++)*10;
            if (*(buf_poz)==0xff) return false;
            *hh+=char_to_int(buf_poz++);
            if (*(buf_poz)==0xff) return false;

            *mm=char_to_int(buf_poz++)*10;
            if (*(buf_poz)==0xff) return false;
            *mm+=char_to_int(buf_poz++);
            if (*(buf_poz)==0xff) return false;

            *ss=char_to_int(buf_poz++)*10;
            if (*(buf_poz)==0xff) return false;
            *ss+=char_to_int(buf_poz++);
            if (*(buf_poz)==0xff) return false;
						buf_poz+=4;
					
            if (*(buf_poz++)!=',') break;
            if (*(buf_poz)==0xff) return false;

            //4807.038
            *N=char_to_int(buf_poz++)*10;
            if (*(buf_poz)==0xff) return false;
            *N+=char_to_int(buf_poz++);
            if (*(buf_poz)==0xff) return false;
            minuts=char_to_int(buf_poz++)*10;
            if (*(buf_poz)==0xff) return false;
            minuts+=char_to_int(buf_poz++);
            if (*(buf_poz)==0xff) return false;
            if (*(buf_poz++)!='.') break;
            if (*(buf_poz)==0xff) return false;
            koef=0.1;
            while (*(buf_poz)!=',')
            {
                minuts+=char_to_int(buf_poz++)*koef;
                if (*(buf_poz)==0xff) return false;
                koef/=10;
            }
            *N+=minuts/60.;
            if (*(buf_poz++)!=',') break;
            if (*(buf_poz)==0xff) return false;
            if (*(buf_poz++)=='S') *N=-(*N);
            if (*(buf_poz)==0xff) return false;
            if (*(buf_poz++)!=',') break;
            if (*(buf_poz)==0xff) return false;


            *E=char_to_int(buf_poz++)*100;
            if (*(buf_poz)==0xff) return false;
            *E+=char_to_int(buf_poz++)*10;
            if (*(buf_poz)==0xff) return false;
            *E+=char_to_int(buf_poz++);
            if (*(buf_poz)==0xff) return false;
            minuts=char_to_int(buf_poz++)*10;
            if (*(buf_poz)==0xff) return false;
            minuts+=char_to_int(buf_poz++);
            if (*(buf_poz)==0xff) return false;
            if (*(buf_poz++)!='.') break;
            if (*(buf_poz)==0xff) return false;
            koef=0.1;
            while (*(buf_poz)!=',')
            {
                minuts+=char_to_int(buf_poz++)*koef;
                if (*(buf_poz)==0xff) return false;
                koef/=10;
            }
            *E+=minuts/60.;
            if (*(buf_poz++)!=',') break;
            if (*(buf_poz)==0xff) return false;
            if (*(buf_poz++)=='W') *E=-(*E);
            if (*(buf_poz)==0xff) return false;
            if (*(buf_poz++)!=',') break;
            if (*(buf_poz)==0xff) return false;

						if (*(buf_poz++)=='0') break;
            if (*(buf_poz)==0xff) return false;
            if (*(buf_poz++)!=',') break;
            if (*(buf_poz)==0xff) return false;
						/*
						
						
            minuts=0;
            while (*(buf_poz)!='.')
            {
                minuts=minuts*10+char_to_int(buf_poz++);
                if (*(buf_poz)==0xff) return false;
            }
            koef=0.1;
            buf_poz++;
            while (*(buf_poz)!=',')
            {
                minuts+=char_to_int(buf_poz++)*koef;
                if (*(buf_poz)==0xff) return false;
                koef/=10;
            }
            *V=minuts;

            buf_poz++;
            minuts=0;
            while (*(buf_poz)!='.')
            {
                minuts=minuts*10+char_to_int(buf_poz++);
                if (*(buf_poz)==0xff) return false;
            }
            koef=0.1;
            buf_poz++;
            while (*(buf_poz)!=',')
            {
                minuts+=char_to_int(buf_poz++)*koef;
                if (*(buf_poz)==0xff) return false;
                koef/=10;
            }
            *K=minuts;
            buf_poz++;


            *DD=char_to_int(buf_poz++)*10;
            if (*(buf_poz)==0xff) return false;
            *DD+=char_to_int(buf_poz++);
            if (*(buf_poz)==0xff) return false;

            *MM=char_to_int(buf_poz++)*10;
            if (*(buf_poz)==0xff) return false;
            *MM+=char_to_int(buf_poz++);
            if (*(buf_poz)==0xff) return false;

            *YY=char_to_int(buf_poz++)*10;
            if (*(buf_poz)==0xff) return false;
            *YY+=char_to_int(buf_poz++);
            if (*(buf_poz)==0xff) return false;
						*/
            return true;



        }
    }
    return false;
}

bool found_gpgll(char*	gprmc_buff, int buff_size,
                 float* N,float* E,float* V, float* K, short* DD,short* MM,short* YY,short* hh,short* mm,short* ss)
{
    int poz;
    char* buf_poz;
    float minuts,koef;
    for (poz=0;poz<buff_size-40;poz++)
    {
        //если нашли заголовок
        if (gprmc_buff[poz]=='$' &&
                gprmc_buff[poz+1]=='G' &&
                gprmc_buff[poz+2]=='P' &&
                gprmc_buff[poz+3]=='G' &&
                gprmc_buff[poz+4]=='L' &&
                gprmc_buff[poz+5]=='L')
        {
            buf_poz=&gprmc_buff[poz+6];
            if (*(buf_poz++)!=',') break;
            // hhmmss
            if (*(buf_poz)==0xff) return false;
            
            *N=char_to_int(buf_poz++)*10;
            if (*(buf_poz)==0xff) return false;
            *N+=char_to_int(buf_poz++);
            if (*(buf_poz)==0xff) return false;
            minuts=char_to_int(buf_poz++)*10;
            if (*(buf_poz)==0xff) return false;
            minuts+=char_to_int(buf_poz++);
            if (*(buf_poz)==0xff) return false;
            if (*(buf_poz++)!='.') break;
            if (*(buf_poz)==0xff) return false;
            koef=0.1;
            while (*(buf_poz)!=',')
            {
                minuts+=char_to_int(buf_poz++)*koef;
                if (*(buf_poz)==0xff) return false;
                koef/=10;
            }
            *N+=minuts/60.;
            
						if (*(buf_poz++)!=',') break;
            if (*(buf_poz)==0xff) return false;
            if (*(buf_poz++)=='S') *N=-(*N);
            if (*(buf_poz)==0xff) return false;
            if (*(buf_poz++)!=',') break;
            if (*(buf_poz)==0xff) return false;


            *E=char_to_int(buf_poz++)*100;
            if (*(buf_poz)==0xff) return false;
            *E+=char_to_int(buf_poz++)*10;
            if (*(buf_poz)==0xff) return false;
            *E+=char_to_int(buf_poz++);
            if (*(buf_poz)==0xff) return false;
            minuts=char_to_int(buf_poz++)*10;
            if (*(buf_poz)==0xff) return false;
            minuts+=char_to_int(buf_poz++);
            if (*(buf_poz)==0xff) return false;
            if (*(buf_poz++)!='.') break;
            if (*(buf_poz)==0xff) return false;
            koef=0.1;
            while (*(buf_poz)!=',')
            {
                minuts+=char_to_int(buf_poz++)*koef;
                if (*(buf_poz)==0xff) return false;
                koef/=10;
            }
            *E+=minuts/60.;
            if (*(buf_poz++)!=',') break;
            if (*(buf_poz)==0xff) return false;
            if (*(buf_poz++)=='W') *E=-(*E);
            if (*(buf_poz)==0xff) return false;
            if (*(buf_poz++)!=',') break;
            if (*(buf_poz)==0xff) return false;

            *hh=char_to_int(buf_poz++)*10;
            if (*(buf_poz)==0xff) return false;
            *hh+=char_to_int(buf_poz++);
            if (*(buf_poz)==0xff) return false;

            *mm=char_to_int(buf_poz++)*10;
            if (*(buf_poz)==0xff) return false;
            *mm+=char_to_int(buf_poz++);
            if (*(buf_poz)==0xff) return false;

            *ss=char_to_int(buf_poz++)*10;
            if (*(buf_poz)==0xff) return false;
            *ss+=char_to_int(buf_poz++);
            if (*(buf_poz)==0xff) return false;

            if (*(buf_poz++)!=',') break;
            if (*(buf_poz)==0xff) return false;
            if (*(buf_poz++)!='A') break;
            if (*(buf_poz)==0xff) return false;
            if (*(buf_poz++)!=',') break;
            if (*(buf_poz)==0xff) return false;

            return true;



        }
    }
    return false;
}

bool found_navi_pack(char*	data_buff, int buff_size)
{
    int poz,ii;
    char* buf_poz;
    float minuts,koef;
		char out_buf_2[128];
    for (poz=0;poz<buff_size-40;poz++)
    {
        //если нашли заголовок
        if (data_buff[poz]=='$')
				{
					if(data_buff[poz+1]=='G' &&
                data_buff[poz+2]=='P' &&
                data_buff[poz+3]=='R' &&
                data_buff[poz+4]=='M' &&
                data_buff[poz+5]=='C') break;
					
					if(data_buff[poz+1]=='G' &&
                data_buff[poz+2]=='P' &&
                data_buff[poz+3]=='G' &&
                data_buff[poz+4]=='L' &&
                data_buff[poz+5]=='L') break;
					
					if(data_buff[poz+1]=='G' &&
                data_buff[poz+2]=='P' &&
                data_buff[poz+3]=='G' &&
                data_buff[poz+4]=='S' &&
                data_buff[poz+5]=='V') break;
					/*
					if(data_buff[poz+1]=='G' &&
                data_buff[poz+2]=='N' &&
                data_buff[poz+3]=='G' &&
                data_buff[poz+4]=='S' &&
                data_buff[poz+5]=='A') break;
					if(data_buff[poz+1]=='G' &&
                data_buff[poz+2]=='P' &&
                data_buff[poz+3]=='V' &&
                data_buff[poz+4]=='T' &&
                data_buff[poz+5]=='G') break;
		if(data_buff[poz+1]=='G' &&
                data_buff[poz+2]=='P' &&
                data_buff[poz+3]=='G' &&
                data_buff[poz+4]=='G' &&
                data_buff[poz+5]=='A') break;
		
		if(data_buff[poz+1]=='G' &&
                data_buff[poz+2]=='L' &&
                data_buff[poz+3]=='G' &&
                data_buff[poz+4]=='S' &&
                data_buff[poz+5]=='V') break;
		
		if(data_buff[poz+1]=='P' &&
                data_buff[poz+2]=='S' &&
                data_buff[poz+3]=='T' &&
                data_buff[poz+4]=='M' &&
                data_buff[poz+5]=='S')
					{
						buf_poz=&data_buff[poz+1];
						ii=0;
						while ((*buf_poz)!='$' && (*buf_poz)!=0xff && ii<120)
						{
							out_buf_2[ii++]=*(buf_poz++);
						}
						out_buf_2[ii]=0;
						the_printf("%s\n\r",out_buf_2);
						break;
					}	
					*/
					
            the_printf ("found %c%c%c%c%c\n\r",
                        data_buff[poz+1],
                    data_buff[poz+2],
                    data_buff[poz+3],
                    data_buff[poz+4],
                    data_buff[poz+5]);
						buf_poz=&data_buff[poz+1];
						ii=0;
						while ((*buf_poz)!='$' && (*buf_poz)!=0xff && ii<120)
						{
							out_buf_2[ii++]=*(buf_poz++);
						}
						out_buf_2[ii]=0;
						the_printf("%s\n\r",out_buf_2);
						
        
        }
    }

}

void write_nav_data_to_file(FIL* fp,char*	buff_for_write, int*  buff_for_write_level)
{
    FRESULT result;
    float N,E,V,K;
    short DD,MM,YY,hh,mm,ss;
	  bool found_data;

		found_data=true;
	  
	if (found_gprms(buff_for_write,*buff_for_write_level,&N,&E,&V,&K,&DD,&MM,&YY,&hh,&mm,&ss))
		{
			work_gps_str.Latitude=E;
			work_gps_str.Longitude=N;
			work_gps_str.Cource=K;
			work_gps_str.Speed=V;
			work_gps_str.DD=DD;
			work_gps_str.MM=MM;
			work_gps_str.MM=YY;
			work_gps_str.hh=hh;
			work_gps_str.mm=mm;
			work_gps_str.ss=ss;
		}
	
	
    if (found_data || found_gpgll(buff_for_write,*buff_for_write_level,&N,&E,&V,&K,&DD,&MM,&YY,&hh,&mm,&ss)||	
			found_gpgga(buff_for_write,*buff_for_write_level,&N,&E,&V,&K,&DD,&MM,&YY,&hh,&mm,&ss))
    {
        the_printf("Navigation %f,%f,%f,%f, %2d/%2d/%2d, %2d:%2d:%2d \n\r",N,E,V,K,DD,MM,YY,hh,mm,ss);
			  total_write_navi_byte+=print_to_file(fp,"%f,\t%f,\t%f,\t%f,\t %d,\t %d,\t %f,\t %d/%d/%d,\t %d:%d:%d,\t%f\n",
			work_gps_str.Longitude,
			work_gps_str.Latitude,
			work_gps_str.Speed,
			work_gps_str.Cource,
			work_gps_str.Number_of_Satellites,
			work_gps_str.HDOP,
			work_gps_str.Altitude,
			work_gps_str.DD,
			work_gps_str.MM,
			work_gps_str.MM,
			work_gps_str.hh,
			work_gps_str.mm,
			work_gps_str.ss,
			work_gps_str.Distance); 
				
    }
    else
    {
        the_printf("Invalid GPS data\n\r");
			found_navi_pack(buff_for_write,*buff_for_write_level);
    }
		/*
    if (fp==0)
    {
        *buff_for_write_level=0;
        return;
    }

    result=f_write(fp, buff_for_write, *buff_for_write_level, &nWritten);
    if (result!=FR_OK)
    {
        the_printf("Error to write data to file, error %d\n",result);
        open_new_file(fp);
    }
    total_write_navi_byte+=nWritten;
    f_sync(fp);
		*/
    if (total_write_navi_byte>max_file_size) open_new_file(fp);

    *buff_for_write_level=0;
}

void reset_max14502()
{
    long int ifor;
    GPIO_ResetBits(GPIOD, GPIO_Pin_15);
    for(ifor=0; ifor<500000; ifor++);
    GPIO_SetBits(GPIOD, GPIO_Pin_15);
    for(ifor=0; ifor<500000; ifor++);
}

void reset_sd_card()
{
	/*
    long int ifor;
    GPIO_ResetBits(GPIOD,GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7);
    for(ifor=0; ifor<500; ifor++);
    GPIO_SetBits(GPIOD,GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_7);
    for(ifor=0; ifor<500; ifor++);
	*/
    reset_max14502();
	
}



void enable_sd_card_mode(FATFS* FATFS_Obj,FIL* fp)
{
    long int ifor;
    GPIO_ResetBits(GPIOD, GPIO_Pin_14 | GPIO_Pin_6);
    reset_sd_card();
    for(ifor=0; ifor<500; ifor++);
    the_printf("SD MODE : SD_CADR\n");
    init_SD_fat(FATFS_Obj,fp);
}

void desable_sd_card_mode(FIL* fp)
{
    long int ifor;
    if (fp!=0)
    {
        f_sync(fp);
        f_close(fp);
    }
    f_mount(0, "0", 1);
    GPIO_SetBits(GPIOD, GPIO_Pin_14 | GPIO_Pin_6);
    reset_sd_card();
    for(ifor=0; ifor<500; ifor++);
    the_printf("SD MODE : USB_FLASH\n");
}



void init_SD_fat(FATFS* FATFS_Obj,FIL* fp)
{
    FRESULT result;
    //long int ifor;//, ifor1;
    DIR dir;
    FILINFO fileInfo;


    reinit_sd_count = 0;
    //GPIO_ResetBits(GPIOD, GPIO_Pin_14);
    //GPIO_SetBits(GPIOD, GPIO_Pin_15);

    the_printf("Try open SDcard\n");

    //if (FATFS_Obj==0)
    //{
        result = f_mount(FATFS_Obj, "0", 1);
        if (result != FR_OK)
        {
            the_printf("Ошибка монтирования диска %d\r\n", result);
            the_printf("Error open SDcard\n");
            reset_max14502();
            FATFS_Obj=0;
        }
        else
        {
            the_printf("SDcard found :)\n");

            result = f_opendir(&dir, "/");
            if (result == FR_OK)
            {
                while (((result = f_readdir(&dir, &fileInfo)) == FR_OK) && fileInfo.fname[0] && nFiles<0xFF)
                {
                    nFiles++;
                    the_printf("%d %s %u\n",nFiles,fileInfo.fname, fileInfo.fsize);


                    if (nFiles>=0xFF)
                    {
                        sprintf(str_buff,"Too many files in directory \n");
                        the_printf(str_buff);
                    }
                    //Debag_printf_uart1(fileInfo.fname);
                    //Debag_printf_uart1("\n");
                }
            }
            f_closedir(&dir);
            file_index=1;

            // открываем файл readme.txt для чтения

            open_new_file(fp);
        }
    //}
}

int main(void)
{
    int ifor;//,ifor2;
    bool last_usb_status;
    bool have_valid_gps_data = true;	
    uint16_t USB_Pin = GPIO_Pin_0;
    FIL str_file;
    FATFS str_FATFS_Obj;
	
#ifndef POPLAVOK
    bool use_tdc;
    status_struct *global_status;
    double t_dir,t_back;
    double* v_spline;
    double tdc_calibrate;
    short iter_index=0;
    USB_Pin = GPIO_Pin_3;
#endif

    memset(&gps_data, 0, sizeof(gps_struct));
    gps_data.valid_data = false;
    
    
    buff_1 = (char*)malloc(max_iq_b_size + 1);
    buff_2 = (char*)malloc(max_iq_b_size + 1);
    buff_1[max_iq_b_size] = 0xff;
    buff_2[max_iq_b_size] = 0xff;
    
    in_array_1_level = 0;
    in_array_2_level = 0;
    in_array_work = in_array_2;
    in_array_work_level = &in_array_2_level;
    
    /*
    out_array_1_level=0;
    out_array_2_level=0;
    out_array_work=out_array_2;
    out_array_work_level=&out_array_2_level;
			*/

#ifndef POPLAVOK
    global_status = (status_struct*)malloc(sizeof(status_struct));
    v_spline = (double*)malloc(sizeof(double) * speed_spline * 6);
#endif

    buff_1_level = 0;
    buff_2_level = 0;
    
    buff_work = buff_1;
    buff_work_level = &buff_1_level;
    
    
    __enable_irq();
    Init_RCC();
    Init_gpio(USB_Pin);
    Init_USART1(115200);
    
#ifndef POPLAVOK
    RS485_init(115200);
    the_printf("Init RS485 done\n");
#endif
    
    the_printf("Init usart2 \n");
    Init_USART2_NAV(115200);
    the_printf("Init usart2 done\n");
    
#ifndef POPLAVOK
    the_printf("Init Keller\n");
    Keller_init();
    the_printf("Init Keller done\n");
    the_printf("Init I2C_Orient\n");
    
    if (Orient_init() == 0) {
      the_printf("Init I2C_Orient done\n");
		}
		else {
			the_printf("Init I2C_Orient error\n");	
		}
    
    /*! --------- TDC initialization ----------- !*/
    the_printf("Init TDC_SPI_Init\n");
    TDC_SPI_Init();
    the_printf("Init TDC_SPI_Init done\n");
    
#endif
    
    the_printf("Init SD SPI\n");
    Init_SPI_SD();
    the_printf("Init SD SPI done\n");

		//while(true)
		//{
		//	Send_data_to_nav("$PSTMSETCONSTMASK,3",7);
		//}

    if (GPIO_ReadInputDataBit(GPIOD, USB_Pin)==0) {
        the_printf("USB desable \n\r");
        last_usb_status=true;
        the_printf("CARD desable \n\r");
        enable_sd_card_mode(&str_FATFS_Obj,&str_file);
    }
    else {
        the_printf("USB enable \n\r");
        reinit_sd_count=0;
        last_usb_status=false;
        the_printf("CARD enable \n\r");
        desable_sd_card_mode(0);
        reset_max14502();
        reset_sd_card();
    }

#ifndef POPLAVOK
		
    Keller_read_data(&global_status->keller_data);
    
    the_printf("Keller %d:PRES %d:TEMP\n", global_status->keller_data.pressure_data, global_status->keller_data.temp_data);
    //RS485_printf("Keller %f:PRES %f:TEMP\n\r", k_pres, k_temp);
    
    if (Orient_read_data(&global_status->orient_data) != 0) {
        the_printf("Orient read : error to read\n\r");
    }
    else {
        the_printf("Orient read :\n\r x_ax: %d\n\r y_ax: %d\n\r z_ax: %d\n\r",
                   global_status->orient_data.x_angular_rate_data,
                   global_status->orient_data.y_angular_rate_data,
                   global_status->orient_data.z_angular_rate_data);
        
        the_printf("x_m: %d\n\r y_m: %d\n\r z_m: %d\n\r",
                   global_status->orient_data.x_axis_magnetic_data,
                   global_status->orient_data.y_axis_magnetic_data,
                   global_status->orient_data.z_axis_magnetic_data);
        
        the_printf("x_a: %d\n\r y_a: %d\n\r z_a: %d\n\r",
                   global_status->orient_data.x_acceleration_data,
                   global_status->orient_data.y_acceleration_data,
                   global_status->orient_data.z_acceleration_data);

        the_printf("temp: %d\n\r",global_status->orient_data.temp_data);
    }
    
    /*! свиридов. !*/
//    tdc_calibrate=TDC_Calibrate(-3.e-6,50,100,dis1,dis2,dis3);
    
#endif

    //основное тело
    while(1) {
				
      //USART_2
        GPIO_ResetBits(GPIOD, GPIO_Pin_0);    //TXEN(DE) D0
        GPIO_ResetBits(GPIOD, GPIO_Pin_1);  	//RXEN(RE) D1	  

        //тупо спим
				if (in_array_1_level>0)
				{
					//in_array_work=in_array_2;
					//in_array_work_level=&in_array_2_level;
					Send_data_to_nav(in_array_1,in_array_1_level);
					in_array_1_level=0;
				}
				
				if (in_array_2_level>0)
				{
					//in_array_work=in_array_1;
					//in_array_work_level=&in_array_1_level;
					Send_data_to_nav(in_array_2,in_array_2_level);
					in_array_2_level=0;
				}
			
        if (GPIO_ReadInputDataBit(GPIOD, USB_Pin)==0)
        {
            if (!last_usb_status) {
              
                the_printf("USB desable \n\r");
                last_usb_status=true;
                the_printf("CARD desable \n\r");
                enable_sd_card_mode(&str_FATFS_Obj,&str_file);
                reset_max14502();
            }
        }
        else {
          
            reinit_sd_count=0;
            if (last_usb_status)
            {
                the_printf("USB enable \n\r");
                last_usb_status=false;
                the_printf("CARD enable \n\r");
                desable_sd_card_mode(&str_file);
                reset_sd_card();

            }
						//continue;

        }


#ifndef POPLAVOK
        
				Keller_read_data(&global_status->keller_data);
        
        the_printf("Keller %d:PRES %d:TEMP\n", global_status->keller_data.pressure_data, global_status->keller_data.temp_data);
        
				
				//RS485_printf("Keller %f:PRES %f:TEMP\n\r", k_pres, k_temp);
        if (Orient_read_data(&global_status->orient_data) != 0) {
          
            the_printf("Orient read : error to read\n\r");
        }
        else {
          
            the_printf("Orient read :\n\r x_ax: %d\n\r y_ax: %d\n\r z_ax: %d\n\r",
                       global_status->orient_data.x_angular_rate_data,
                       global_status->orient_data.y_angular_rate_data,
                       global_status->orient_data.z_angular_rate_data);
            
            the_printf("x_m: %d\n\r y_m: %d\n\r z_m: %d\n\r",
                       global_status->orient_data.x_axis_magnetic_data,
                       global_status->orient_data.y_axis_magnetic_data,
                       global_status->orient_data.z_axis_magnetic_data);
            
            the_printf("x_a: %d\n\r y_a: %d\n\r z_a: %d\n\r",
                       global_status->orient_data.x_acceleration_data,
                       global_status->orient_data.y_acceleration_data,
                       global_status->orient_data.z_acceleration_data);

            the_printf("temp: %d\n\r",global_status->orient_data.temp_data);
        }

				iter_index++;
				if (iter_index>=speed_spline) iter_index=0;
				
        /*! ----- Getting TOF ------- !*/
        if (TDC_GetTOF(0, &t_dir, &t_back, tdc_calibrate) == 0) {
          
            v_spline[iter_index] = t_dir;
            v_spline[iter_index + speed_spline] = t_back;
            t_dir = v_spline[0];
            t_back = v_spline[speed_spline];
            
            for (ifor = 1; ifor<speed_spline; ifor++) {
              
            	t_dir += v_spline[ifor];
            	t_back += v_spline[ifor + speed_spline];
            }
            
            t_dir /= speed_spline;
            t_back /= speed_spline;
            
            global_status->tdc_data.tof_1_dir = t_dir;
            global_status->tdc_data.tof_1_back = t_back;
            global_status->tdc_data.v_1_dir = dis1 / t_dir;
            global_status->tdc_data.v_1_back = dis1 / t_back;
            
            the_printf("TOF 1 %f(V=%f) %f(V=%f) %g\n\r",
                       t_dir,
                       dis1/t_dir,
                       t_back,
                       dis1/t_back,
											dis1/t_dir-dis1/t_back);
        }
        
        else { 
          
            global_status->tdc_data.tof_1_dir=-1;
            global_status->tdc_data.tof_1_back=-1;
					  global_status->tdc_data.v_1_dir=-1;
            global_status->tdc_data.v_1_back=-1;
            
            the_printf("Error to get TOF 1\n\r");
        }

        if (TDC_GetTOF(2, &t_dir, &t_back, tdc_calibrate)==0) {
					
          if (fabs(dis2 / t_dir - dis2 / t_back) > 30)
					{
						if (iter_index==0)
						{
							v_spline[iter_index+speed_spline*2]=v_spline[iter_index+speed_spline*3-1];
							v_spline[iter_index+speed_spline*3]=v_spline[iter_index+speed_spline*4-1];
						}
						else
						{
							v_spline[iter_index+speed_spline*2]=v_spline[iter_index+speed_spline*2-1];
							v_spline[iter_index+speed_spline*3]=v_spline[iter_index+speed_spline*3-1];
						}
					}
					else
					{
						v_spline[iter_index+speed_spline*2]=t_dir;
						v_spline[iter_index+speed_spline*3]=t_back;
					}
					t_dir=v_spline[speed_spline*2];
					t_back=v_spline[speed_spline*3];
					for (ifor=1;ifor<speed_spline;ifor++)
					{
						t_dir+=v_spline[ifor+speed_spline*2];
						t_back+=v_spline[ifor+speed_spline*3];
					}
					t_dir/=speed_spline;
					t_back/=speed_spline;
          
            global_status->tdc_data.tof_2_dir=t_dir;
            global_status->tdc_data.tof_2_back=t_back;
            global_status->tdc_data.v_2_dir=dis2/t_dir;
            global_status->tdc_data.v_2_back=dis2/t_back;
            the_printf("TOF 2 %f(V=%f) %f(V=%f) %g\n\r",
                       t_dir,
                       dis2/t_dir,
                       t_back,
                       dis2/t_back,
						dis2/t_dir-dis2/t_back);
        }
        else
        {
					global_status->tdc_data.tof_2_dir=-1;
            global_status->tdc_data.tof_2_back=-1;
					  global_status->tdc_data.v_2_dir=-1;
            global_status->tdc_data.v_2_back=-1;
            the_printf("Error to get TOF 2\n\r");
        }

        if (TDC_GetTOF(4,&t_dir,&t_back,tdc_calibrate)==0)
        {
					if (fabs(dis1/t_dir-dis1/t_back)>30)
					{
						if (iter_index==0)
						{
							v_spline[iter_index+speed_spline*4]=v_spline[iter_index+speed_spline*5-1];
							v_spline[iter_index+speed_spline*5]=v_spline[iter_index+speed_spline*6-1];
						}
						else
						{
							v_spline[iter_index+speed_spline*4]=v_spline[iter_index+speed_spline*4-1];
							v_spline[iter_index+speed_spline*5]=v_spline[iter_index+speed_spline*5-1];
						}
					}
					else
					{
						v_spline[iter_index+speed_spline*4]=t_dir;
						v_spline[iter_index+speed_spline*5]=t_back;
					}
					t_dir=v_spline[speed_spline*4];
					t_back=v_spline[speed_spline*5];
					for (ifor=1;ifor<speed_spline;ifor++)
					{
						t_dir+=v_spline[ifor+speed_spline*4];
						t_back+=v_spline[ifor+speed_spline*5];
					}
					t_dir/=speed_spline;
					t_back/=speed_spline;
          
            global_status->tdc_data.tof_3_dir=t_dir;
            global_status->tdc_data.tof_3_back=t_back;
					  global_status->tdc_data.v_3_dir=dis3/t_dir;
            global_status->tdc_data.v_3_back=dis3/t_back;
            
            the_printf("TOF 3 %f(V=%f) %f(V=%f) %g\n\r",
                       t_dir,
                       dis3/t_dir,
                       t_back,
                       dis3/t_back,
						dis3/t_dir-dis3/t_back);
        }
        else
        {
					global_status->tdc_data.tof_3_dir=-1;
            global_status->tdc_data.tof_3_back=-1;
					  global_status->tdc_data.v_3_dir=-1;
            global_status->tdc_data.v_3_back=-1;
            
            the_printf("Error to get TOF 3\n\r");
        }
        
        
        RS485_send_status(global_status);

#endif

        /*Навигация пытаемся писать*/

        if (have_valid_gps_data)
        {
            if (buff_1_level>=max_iq_b_size)
            {
                write_nav_data_to_file(&str_file,buff_1,&buff_1_level);
            }

            if (buff_2_level>=max_iq_b_size)
            {
                write_nav_data_to_file(&str_file,buff_2,&buff_2_level);
            }
        }
        else
        {
            buff_1_level=0;
            buff_2_level=0;
        }
    }
}





int the_printf(const char *str, ...)
{

    char info_string[128];
    va_list ap;
    va_start(ap,str); // начало параметров
    vsprintf(info_string,str,ap);
    va_end(ap);     // очистка параметров
#ifndef POPLAVOK
    RS485_printf("%s\r",info_string);
#endif
    return Debag_printf_uart1(info_string);
}




int Debag_printf_uart1(char *str)
{
    //while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == SET);
    UINT sb=0;
		#ifndef POPLAVOK

    while(*str != '\0')
    {
        sb++;
        USART_ClearFlag(USART1, USART_FLAG_TC);

        USART_SendData(USART1, *(str++));
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET);
    }
    USART_SendData(USART1, 13);
    sb++;
		#endif
    return sb;
		
}

void Send_data_to_nav(char *str,int size_to_send)
{
    //while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == SET);
    UINT sb=0;

    while(sb<size_to_send)
    {
        USART_ClearFlag(USART2, USART_FLAG_TC);
        USART_SendData(USART2, str[sb]);
	   		sb++;

        while (USART_GetFlagStatus(USART2, USART_FLAG_TC) != SET);
    }		
}

void Send_data_from_nav(char *str,int size_to_send)
{
    //while (USART_GetFlagStatus(USART1, USART_FLAG_TC) == SET);
    UINT sb=0;

    while(sb<size_to_send)
    {
        USART_ClearFlag(USART1, USART_FLAG_TC);
        USART_SendData(USART1, str[sb]);
				sb++;
        while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET);
    }		
}

void USART1_IRQHandler(void)
{
    char dat;
    //UINT nWritten;
    if(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==SET)
    {
        dat = USART_ReceiveData(USART1); //очистить флаги если это не делается аппаратно(проверить)+ дописать вход для обработчика прерываний на отправку
        USART_ClearFlag(USART1,USART_FLAG_RXNE);
        //дальще dat можно девать куда угодно
			in_array_work[in_array_level]=dat;	
			in_array_level++;
			if (in_array_level>2 && dat==0x0A)
			{
				if (in_array_work[in_array_level-2]==0x0D)	
				{
				*in_array_work_level=in_array_level;
				if (in_array_work==in_array_1)
				{
					in_array_work=in_array_2;
					in_array_work_level=&in_array_2_level;
				}
				else
					{
					in_array_work=in_array_1;
					in_array_work_level=&in_array_1_level;
				}
					in_array_level=0;
				}
			}
    }
}


void USART2_IRQHandler(void)
{
    char dat;
    //UINT nWritten;
    if(USART_GetFlagStatus(USART2,USART_FLAG_RXNE)==SET)
    {
        dat = USART_ReceiveData(USART2); //очистить флаги если это не делается аппаратно(проверить)+ дописать вход для обработчика прерываний на отправку
        USART_ClearFlag(USART2,USART_FLAG_RXNE);
        //дальще dat можно девать куда угодно
			
#ifdef POPLAVOK
			//out_array_work[*out_array_work_level]=dat;	
			//(*out_array_work_level)++; 
			
			USART_ClearFlag(USART1, USART_FLAG_TC);
      USART_SendData(USART1, dat);
      //while (USART_GetFlagStatus(USART1, USART_FLAG_TC) != SET);
#endif
			
        if (*buff_work_level<max_iq_b_size)
        {
            buff_work[*buff_work_level]=dat;
            *buff_work_level=(*buff_work_level)+1;
        }
        if (*buff_work_level==max_iq_b_size)
        {
            if (buff_work==buff_1)
            {
                buff_work=buff_2;
                buff_work_level=&buff_2_level;
            }
            else
            {
                buff_work=buff_1;
                buff_work_level=&buff_1_level;

            }
        }

        /*
                USART_SendData(USART1, dat);

                if (total_write_byte<0) return;
                f_write(&file, &dat, 1, &nWritten);
                total_write_byte+=nWritten;
                if (total_write_byte>16*1024*1024) open_new_file(&file);
                */
    }
}

void UART5_IRQHandler (void)
{
    //char dat;
    if(USART_GetFlagStatus(UART5,USART_FLAG_RXNE)==SET)
    {
        /*dat = */USART_ReceiveData(UART5); //очистить флаги если это не делается аппаратно(проверить)+ дописать вход для обработчика прерываний на отправку
        USART_ClearFlag(UART5,USART_FLAG_RXNE);
        //дальще dat можно девать куда угодно

    }
}

void SPI2_IRQHandler (void)
{
    //uint16_t data;
    if(SPI_I2S_GetFlagStatus(SPI2, SPI_I2S_FLAG_RXNE) == SET)
    {
        /*data = */SPI_I2S_ReceiveData(SPI2);

        SPI_I2S_ClearFlag(SPI3, SPI_I2S_FLAG_RXNE);

        GPIO_SetBits(GPIOD, GPIO_Pin_0);	//опускаем NSS // у вас будет другой пин

    }

}
