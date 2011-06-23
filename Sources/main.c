/******************************  
Port Definition:
PORT A  :   LED
PORT B  :   OV7620 data
PP1     :   Servo 
PP5     :   Cam_Servo     
PE0:PE7 :   DIP_SWICH
PP0,PP2 :   mma7260 range
PP6     :   mma7260 sleep
PP3     :   Motor run
PP7     :   Motor break
ptad04  :   slop_sensor
PS0PS1  :   Bluetooth with uart
ATD2    :   CamServo position
ATD0    :   Acceleration sensor
*******************************/

#include "includes.h"

#pragma CODE_SEG DEFAULT


int i = 0;
int j = 0;

void SendImage(void);
void SendImage2(void);
void SendInfo(void);
char dec_to_char(int n);

void main(void)
{
    DisableInterrupts;

    MCU_Init();
    WaitForStart();
    
    EnableInterrupts;
    
    for(;;)  
    {    
        if( flag == 1 )
        {
            DisableInterrupts;
            TFLG1 = 0x00;
            flag = 0;
            ImageProcess();
            if( (routetype == 0 || routetype == 1) && flag_start == 0 ) //in straight road, and didn't find startline yet.
            {
                if( dip7 == 1 )
                {                    
                    Check_start();
                }
            }
            CamControl();
            AutoControl();
            
            if(debug)
            {
                //SendInfo();
                SendImage2();
            }
            TFLG1 = 0x0F;
            EnableInterrupts;
        }
        if( !debug )
        {            
            if( controlspeed )
            {
                DisableInterrupts;
                SpeedControl();
                controlspeed = 0;
                EnableInterrupts;
            }
        }
    }
}

char dec_to_char(int n)   
{   
    return "0123456789ABCDEF"[n];   
}

void SendImage(void)
{    
    //print gray scale image
    uart_putchar('i');
    uart_putchar('m');
    uart_putchar('g');
    uart_putchar(0x0D);
    for( i = 0; i < ROW_VALUE; i++ )
    {
        PORTA_PA0 = !PORTA_PA0;
        for( j = 0; j < COLUMN_VALUE; j++ )
        {
            uart_putchar(buff[i][j]);
            
        }
    }
    uart_putchar(0x0D);
}

void SendImage2(void)   //for hand terminal
{    
    //send gray scale image
    uart_putchar('i');
    uart_putchar('m');
    uart_putchar('g');
    uart_putchar(0x0D);
    for( i = 0; i < 10000; i++ );
    for( i = 0; i < ROW_VALUE; i++ )
    {
        PORTA_PA0 = !PORTA_PA0;
        for( j = 0; j < COLUMN_VALUE; j++ )
        {
            uart_putchar(buff[i][j]);
        }
    }
    uart_putchar(0x0D);
    
    for( i = 0; i < 10000; i++ )
        for( j = 0; j < 500; j++ );
    
    //send center
    uart_putchar('c');
    uart_putchar('e');
    uart_putchar('t');
    uart_putchar(0x0D);
    for( i = 0; i < ROW_VALUE; i++ )
    {
        uart_putchar( (unsigned char)Line_Center[i] );
    }
    uart_putchar(0x0D);
    
    for( i = 0; i < 10000; i++ )
        for( j = 0; j < 500; j++ );
}


void SendInfo(void)
{   
    /*
    uart_putchar('p');
    uart_putchar(' ');
    uart_putchar( prediction/10 + 0x30 );
    uart_putchar( prediction%10 + 0x30 );
    uart_putchar(' ');
    uart_putchar(' ');
    uart_putchar('f');
    uart_putchar('c');
    uart_putchar(' ');
    uart_putchar( (farcurve/100)%10 + 0x30 );
    uart_putchar( (farcurve/10)%10 + 0x30 );
    uart_putchar( (farcurve%10) + 0x30 );
    uart_putchar(' ');
    uart_putchar(' ');
    uart_putchar('p');
    uart_putchar('d');
    uart_putchar(' ');
    if( slope_product_near < 0 )
    {
        uart_putchar('-');
    }
    uart_putchar( abs(slope_product_near/100)%10 + 0x30 );
    uart_putchar( abs(slope_product_near/10)%10 + 0x30 );
    uart_putchar( abs(slope_product_near%10) + 0x30 );
    uart_putchar('\n');
    */
    uart_putchar('v');
    uart_putchar('p');
    uart_putchar(' ');
    uart_putchar( valid_prediction_value/100%10 + 0x30 );
    uart_putchar( valid_prediction_value/10%10 + 0x30 );
    uart_putchar( valid_prediction_value%10 + 0x30 );
    //uart_putchar('\n');
    uart_putchar(0x0D);
}
