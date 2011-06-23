#include "includes.h"

#pragma CODE_SEG __NEAR_SEG NON_BANKED

int isbreak = 0;

void interrupt 7 RTI_interrupt(void) //不能嵌套，所以得到的20ms并不准确
{
    CRGFLG |= 0x80;  
    pulse_counter = PACNT;  
    PACNT = 0;
    controlspeed = 1;
}

void interrupt 8 Port0_interrupt(void) // HS interrupt
{
    static unsigned int i;

    TFLG1 = 0x01; // Clear HS flag
    if( g_SampleFlag == 0 ) // 不用这么搞，提前关行终中断就可以
    {
        return;
    }
    row_counter++;
    
    if( row_counter % SAMPLE_INTERVAL == 0 )
    {                                                                                                         
        app = &buff[row][0];
        row++;
        if( row_counter > 200 )
        {
            g_SampleFlag = 0;
            TIE_C0I = 0;
            flag = 1;
            return; //test
            
        }
            
        for( i = 0; i < COLUMN_VALUE; i++ ) 
        {
            _asm(nop);_asm(nop);_asm(nop);
            _asm(nop);_asm(nop);_asm(nop);
            _asm(nop);_asm(nop);_asm(nop);
            _asm(nop);_asm(nop);_asm(nop);
            _asm(nop);_asm(nop);_asm(nop);
            _asm(nop);_asm(nop);_asm(nop);
            //for 80Mhz
            *app++ = PORTB;
        }
    }
}
          
void interrupt 9 Port1_interrupt(void) //FODD interrupt
{ 
    TFLG1 = 0x02; //Clear FODD flag
    g_SampleFlag = 1;
    TIE_C0I = 1;
    row = 0; //Reset row
    row_counter = 0;
    get_lost_time = 0; //Reset get_lost_time
    //SAMPLE_INTERVAL = 5;
    PORTA_PA7 = !PORTA_PA7;
    
    // get pulse every 33.33ms
    pulse_counter = PACNT;  
    PACNT = 0;
    if( !isbreak )
    {
        controlspeed = 1;
    }
    if( flag_start )
    {
        image_counter++;
        
        if( image_counter > 100 )
        {
            flag_start = 0; //permit to check startline
            image_counter = 0;
        }
    }
}

void interrupt 20 RxInterrupt(void)
{
    DisableInterrupts;
    if( SCI0SR1 & 0x20 )
    {
        CaptureCommand = SCI0DRL;
    }
    switch(CaptureCommand)
    {
        case '0':
            BREAK();isbreak = 1;constant_speed = 0;
            break;
        case '9':
            RUN();debug = 0;isbreak = 0;constant_speed = 450;
            break;
        case 'b':
            STOP();constant_speed = 0;
            break;
        case 'c':
            STOP();constant_speed = 0;
            break;
        case 'd':
            debug = 1;STOP();constant_speed = 0;
            break;
        default:
            break;
    }
    EnableInterrupts;
}