#include "includes.h"
 
#pragma CODE_SEG DEFAULT

/***************************************************************
*                    Variables Initialization                  *
***************************************************************/

int Line_Center[ROW_VALUE];
int Line_Center_bak[ROW_VALUE];
unsigned char buff[ROW_VALUE][COLUMN_VALUE];
unsigned char data_b[COLUMN_VALUE];


Bool dip1, dip2, dip3, dip4, dip5, dip6, dip7, dip8;

unsigned char SAMPLE_INTERVAL;                              
unsigned char *app;
unsigned char CaptureCommand;
unsigned char *p_buff;
float threshold, thresholdtemp;
//int RowToProcess;
int blacksum;
int blackposcenter;
int blackposavg;
int blackpossum;
int blackfiltered;
//int lastblack;
int row_counter;
int row;
int steer;
unsigned int pidcounter;
unsigned int RTIcounter;
unsigned int pulse_counter;
unsigned char turn_left, turn_right;
unsigned char get_lost_time;
unsigned char image_lost;
unsigned int stat[16];
unsigned char search_l, search_r;
unsigned char search_range;
unsigned char find_start;
float servo_factor_l, servo_factor_r;
float servo_factor;
float servo_factor_d1;
float servo_factor_d2;
float cam_factor;
unsigned char slop_sensor;
int constant_speed;
int prediction;
int valid_prediction_value;
int pre_valid;   //previous valid_prediction_value
int pre_d_valid;
int slope_var;
int leftpeak, rightpeak;
int leftpeakpos, rightpeakpos;
int max_slope_var_pos;
int farcurve;
int farslope1, farslope2;
int nearcurve;
int nearslope1, nearslope2;
int slope_product_near;
int slope_product_far;
unsigned char nearest_linecenter, previous_nearest_linecenter;
unsigned char permit_nearest_linecenter_err;
int line_dir, pre_line_dir;   //Line_Center[35] - Line_Center[39], avoid being disturbed by cross line
unsigned char permit_line_dir_err;
unsigned char image_core, pre_image_core;   //center of the whole image
unsigned char permit_image_core_err;
int nearest_valid_line;
int continuous_valid_line;
unsigned char slop_sensor;
int overspeedtime;
int routetype;
int scheme;
int counter_straight, counter_ss, counter_bs, counter_left, counter_right, counter_slope;
int edge_ll[ROW_VALUE];  //for start line checking
int edge_rr[ROW_VALUE];  //for start line checking
int start_counter;
int image_counter;

byte pid_flag;//弯度大小标志，用来判断是否pid
unsigned int speed;
unsigned int ideal_speed;//给定的速度值
int pre_error;    //速度PID 前一次的速度误差值
int pre_d_error;  //速度PID 前一次的速度误差之差
int pk;           //给的实际速度，由PID计算得出

int ideal_angle;    //给定的偏移量
int cam_pre_error;    //偏移量PID 前一次的偏移量误差值
int cam_pre_d_error;  //偏移量PID 前一次的偏移量误差之差
int cam_pk;           //给的实际偏移量，由PID计算得出
int currentangle;   //摄像头目前的偏移量
int newangle;    //计算出的新偏移量
int angle;
int campos;  //data base on AD
int pre_campos;
int test;

volatile Bool flag_image_lost;
volatile Bool flag_line_dir_err;
volatile Bool g_SampleFlag;
volatile Bool flag;
volatile Bool findroute;
volatile Bool flag_start;
volatile Bool debug, button1pressed, button2pressed;
volatile Bool TESTSERVO;
volatile Bool controlspeed;



// 舵机转角角度对应表
const int servo_arr[121] =
{
    800,  811,  822,  833,  844,  855,  866,  877,  888,
    900,  911,  922,  933,  944,  955,  966,  977,  988,
    1000, 1011, 1022, 1033, 1044, 1055, 1066, 1077, 1088,
    1100, 1111, 1122, 1133, 1144, 1155, 1166, 1177, 1188,
    1200, 1211, 1222, 1233, 1244, 1255, 1266, 1277, 1288,
    1300, 1311, 1322, 1333, 1344, 1355, 1366, 1377, 1388,
    1400, 1411, 1422, 1433, 1444, 1455, 1466, 1477, 1488,
    1500, 1511, 1522, 1533, 1544, 1555, 1566, 1577, 1588,
    1600, 1611, //servo_arr[64]
    1622, 1633, 1644, 1655, 1666, 1677, 1688, 1700, 1711,
    1722, 1733, 1744, 1755, 1766, 1777, 1788, 1800, 1811,
    1822, 1833, 1844, 1855, 1866, 1877, 1888, 1900, 1911,
    1922, 1933, 1944, 1955, 1966, 1977, 1988, 2000, 2011,
    2022, 2033, 2044, 2055, 2066, 2077, 2088, 2100, 2111,
    2122, 2133
};
//速度对应脉冲表
const int pulse_speed[40] = 
{
    /* old encoder
    // for 33.33ms
    800,550,450,400,380,364,356,348,340,332,
    324,316,308,300,292,284,276,268,260,252,
    244,236,228,220,212,204,196,188,180,172,
    164,156,148,140,132,124,116,108,100,80
    
    // for 50ms
    800,700,600,500,490,480,470,460,450,440,
    430,420,410,400,390,380,370,360,350,340,
    330,320,310,300,290,280,270,260,250,240,
    230,220,210,200,190,180,170,160,150,125
    */
    /*    
    // for 33.33ms new encoder normal
    1200,1100,1050,1000,980,960,940,920,900,880,
    860,840,820,800,780,760,740,720,700,680,
    660,640,620,600,590,580,570,560,550,540,
    530,520,510,500,490,480,470,460,450,400
    */
    /*
    // for 33.33ms new encoder fast
    1500,1475,1450,1400,1300,1200,1100,1050,1000,960,
    940,920,900,880,860,840,820,800,780,760,
    740,720,700,680,660,640,620,600,580,560,
    540,530,520,510,500,490,480,470,460,450
    */
    // for 33.33ms new encoder fast (test)
    1500,1450,1400,1350,1300,1250,1200,1150,1100,1050,
    1000,980,960,940,920,900,880,860,840,820,
    800,750,700,680,660,640,620,600,580,560,
    540,530,520,510,500,490,480,470,460,450
};

/***************************************************************
*                      Modules Initialization                  *
***************************************************************/

void SetBusCLK_80M(void) /* 80MHz */
{   
    CLKSEL = 0x00; /* disengage PLL to system */
    PLLCTL_PLLON = 1; /* turn on PLL */
    SYNR = 0xC0 | 0x09; /****************************************
                         VCOFRQ[7:6];SYNDIV[5:0]
                         fVCO = 2*fOSC*(SYNDIV + 1)/(REFDIV + 1)
                         fPLL = fVCO/(2 * POSTDIV) 
                         fBUS = fPLL/2 
                         VCOCLK Frequency Ranges  VCOFRQ[7:6]
                         32MHz <= fVCO <= 48MHz    00
                         48MHz <  fVCO <= 80MHz    01
                         Reserved                  10
                         80MHz <  fVCO <= 120MHz   11
    	                *****************************************/    
    REFDV = 0x00 | 0x01; /***************************************
    	                  REFFRQ[7:6];REFDIV[5:0]
                          fREF = fOSC/(REFDIV + 1)
                          REFCLK Frequency Ranges  REFFRQ[7:6]
                          1MHz <= fREF <=  2MHz       00
                          2MHz <  fREF <=  6MHz       01
                          6MHz <  fREF <= 12MHz       10
                          fREF >  12MHz               11                         
                          pllclock = 2*osc*(1+SYNDIV)/(1+REFDIV) = 160MHz;
    	                 *****************************************/
    POSTDIV = 0x00; /****************************************
                     4:0, fPLL = fVCO/(2xPOSTDIV)
                     If POSTDIV = $00 then fPLL is identical to fVCO
                                  (divide by one).
                    *****************************************/
    _asm(nop); /* BUS CLOCK = 80M */
    _asm(nop);//fBUS = (1/2)*2*16M*(9+1)/(1+1) = 80M
    _asm(nop);
    _asm(nop); /* nops required for PLL stability */
    while( !CRGFLG_LOCK ); /* when pll is steady ,then use it */
    CLKSEL_PLLSEL = 1; /* engage PLL to system */
}

void SetBusCLK_48M(void) /* 48MHz */
{ 
    CLKSEL = 0x00;
    PLLCTL_PLLON = 1;
    SYNR = 0XC0 | 0X0B;//SYNDIV = 11
    REFDV = 0X00 | 0X03;//REFDIV = 3
    POSTDIV = 0X00;
    _asm(nop);//fBUS = fPLL/2 = fVCO/2 = (1/2)*2*fOSC*(SYNDIV + 1)/(REFDIV + 1)
    _asm(nop);//fBUS = 16M*12/4 = 48M
    _asm(nop);
    _asm(nop);
    while( !CRGFLG_LOCK );
    CLKSEL_PLLSEL = 1;
}

void ATD_Init_Original(void)
{
    ATD0CTL1 = 0x00; /*********************************
                      0b 0000 0000
                      0:External Trigger Source;
                      00:8-bit data;
                      0:No discharge;
                      0000:Channel select AN0
                     *********************************/
                    
    ATD0CTL2 = 0x40; /*********************************
                      0b 0100 0000
                      0:Reserved;
                      1:Set ATD Fast Conversion Complete Flag;
                      0:Internal Clock in Stop Mode Bit;
                      00:External Trigger Edge Control(Falling Edge);
                      0:External Trigger Mode Enable;
                      0:ATD Sequence Complete INT disabled;
                      0:ATD Compare Interrupt disabled;                  
                     *********************************/
                  
    ATD0CTL3 = 0x88; /*********************************
                      0b 1000 1000
                      1:Right justified data
                        in ATD conversion result register(16 bit);
                      0001:Conversion Sequence Length(1 bit);
                      0:Result Register FIFO Mode;
                      00:Continue conversion in Freeze Mode
                      (ATD Behavior in Freeze Mode);                 
                     *********************************/
                   
    ATD0CTL4 = 0x01; /*********************************
                      0b 0000 0001
                      000:4 AD Sample Time
                      (Sample Time in Number of ATD Clock Cycles);
                      00001:ATD Clock Prescaler
                       (f = fbus/[2*(PRS + 1)]);    
                     *********************************/
                    
    ATD0CTL5 = 0x20; /*********************************
                      0b 0010 0000
                      0:Reserved
                      0:Special Channel Conversion Bit disabled;
                      1:Continuous conversion sequence(scan mode);
                      0:Sample Only one channel(Muti-Channel Sample Mode);
                      0000:Analog Channel Select Code(AN0);
                     *********************************/
                    
    ATD0DIEN = 0x00; /* Digital Input Enable on channel x(All disabled) */
}

void ATD_Init(void)
{
    ATD0CTL1 = 0x00;                    
    ATD0CTL2 = 0x40;
    ATD0CTL3 = 0xF0;
    ATD0CTL4 = 0x02;             
    ATD0CTL5 = 0x30;
    ATD0DIEN = 0x00;
}

/*
unsigned char Read_ATD(void)
{
    unsigned char data_low;
    //unsigned char data_high;
    while( !ATD0STAT0_SCF ) //If Sequence Complete Flag == 0,wait forever
    {
        _asm(nop);
    }
    data_low = ATD0DR0L; //Read Lower 8 bits
    //data_high = ATD0DR0H;
    return data_low;
}
*/

unsigned char Read_ATD( unsigned char ch )
{
    unsigned char data_low;
    //unsigned char data_high;
    
    if( ch == 0 )
    {
        while( !ATD0STAT0_SCF ) //If Sequence Complete Flag == 0,wait forever
        {
            _asm(nop);
        }
        data_low = ATD0DR0L; //Read Lower 8 bits
    }
    else if( ch == 2 )
    {
        while( !ATD0STAT0_SCF ) //If Sequence Complete Flag == 0,wait forever
        {
            _asm(nop);
        }
        data_low = ATD0DR2L; //Read Lower 8 bits
    }
    else;
    return data_low;
}

void IRQ_Init(void)
{
    IRQCR_IRQE = 1; /* IRQ select edge sensitive */
                    /* (1:Respond only to falling edges) */
    IRQCR_IRQEN = 0; /* IRQ Enable */
}

void Port_Init(void)
{
    DDRB = 0x00;     
    DDRA = 0xFF;
    PORTA = 0xFF;
    /*
    DDRP_DDRP0 = 1;
    DDRP_DDRP2 = 1;
    DDRP_DDRP6 = 1;
    PTP_PTP0 = 1;//Lagest MMA7260_RANGE
    PTP_PTP2 = 1;//Lagest MMA7260_RANGE
    PTP_PTP6 = 1;//Disable MMA7060_SLEEP
    */
    
    // Dip_Switch_Init
    IRQCR_IRQEN = 0;    //Disable IRQ at PE1
    DDRE = 0X00;    //PE2~PE7 dip_switch
    DDRT_DDRT6 = 1;
    PTT_PTT6 = 1;
    DDRM = 0xFF;    //LEDs
    PTM = 0xFF;
}

void PWM_Init(void)
{
    PWME = 0; // Disable PWM
    
    /* PWM0 & PWM1 concatenate for Servo */
    PWMCTL_CON01 = 1; // Concatenation
    PWMPRCLK_PCKA = 1; // Rate of A = Bus clock/2 (40MHz)
    PWMSCLA = 20; // Clock SA = Clock A / (2 * PWMSCLA) (1MHz)
    PWMPOL_PPOL1 = 1; // high at the beginning
    PWMCLK_PCLK1 = 1; // PWM01 select SA
    PWMPER01 = 20000; // 50Hz [PWMx Period = Channel Clock Period*(2*PWMPERx),1us * 20000 = 20ms]
    PWMDTY01 = SERVO_MID;
    
    PWME_PWME1 = 1; // Enable servo
    
    /* PWM4 & PWM5 concatenate for Cam */
    PWMCTL_CON45 = 1; // Concatenation
    //PWMPRCLK_PCKA = 1; // Rate of A = Bus clock/2 (40MHz)
    //PWMSCLA = 20; // Clock SA = Clock A / (2 * PWMSCLA) (1MHz)
    PWMPOL_PPOL5 = 1; // high at the beginning
    PWMCLK_PCLK5 = 1; // PWM45 select SA
    PWMPER45 = 20000; // 50Hz [PWMx Period = Channel Clock Period*(2*PWMPERx),1us * 20000 = 20ms]
    PWMDTY45 = CAM_SERVO_MID;
    PWME_PWME5 = 1; // Enable cam servo

    /* PWM3 & PWM7 for Motor */
    PWMCTL_CON23 = 1;
    PWMCTL_CON67 = 1;
    PWMPRCLK_PCKB = 1; // Rate of B = Bus clock/32 = 80M/2 = 40M
    PWMSCLB = 20; // SB = B/2*20 = 1M
    PWMPOL_PPOL3 = 1; // PP3 output are high at the beginning
    PWMPOL_PPOL7 = 1; // PP7 output are high at the beginning
    PWMCLK_PCLK3 = 1; // PWM3 select SB
    PWMCLK_PCLK7 = 1; // PWM7 select SB
    PWMPER23 = 10000;  //SB/10000 = 100Hz
    PWMDTY23 = 10000;
    PWMPER67 = 10000;
    PWMDTY67 = 0;
    PWME_PWME3 = 0; //break
    PWME_PWME7 = 1; //run
}

void SCI_Init(void)
{
    SCI0CR1 = 0x04; //9 bits, no parity
    SCI0CR2_RE = 1; //Enable Rxd
    SCI0CR2_TE = 1; //Enable Txd
    SCI0CR2_RIE = 1; //Rxd interrupt
    SCI0CR2_TCIE = 0; //Disable Txd interrupt

    SCI0BDH = 0X00;
    SCI0BDL = 0X82; //SCI baud rate=busclk/(16*SCI0BDL)
                    //busclk  8MHz, 9600bps, SCI0BD=0x34
                    //busclk 16MHz, 9600bps, SCI0BD=0x68
                    //busclk 24MHz, 9600bps, SCI0BD=0x9C
                    //busclk 32MHz, 9600bps, SCI0BD=0xD0
                    //busclk 48MHz, 9600bps, SCI0BD=0x138
                    //busclk 80MHz, 9600bps, SCI0BD=0x208
                    //busclk 80MHz, 38400bps, SCI0BD=0x82
                    //busclk 80Mhz, 115200bps, SCI0BD=0x2B
}

//SCI Transmit
void uart_putchar(unsigned char ch)
{
    while( !(SCI0SR1 & 0x40) );
    SCI0DRL = ch;
}

void TimerInit(void) //HS,VS interrupt Initialization use Input Capture)
{
    TSCR1 = 0x80; //TIMER Enable
    TIE = 0x00; //Interrupt disabled
    TCTL3 = 0x00; //Other channel capture disabled      
    TCTL4 = 0x09; //channel 0 ==> rising edge and channel 1 ==> falling edge
    TIOS = 0x00; //0:Capture mode
    TFLG1 = 0xFF; //Clear interrupt flag
    TIE_C0I = 1; //Enable HS interrupt
    TIE_C1I = 1; //Enable FODD interrupt
}

void TimerAccInit(void)
{
    /* 
    //TSCR2 = 0x04;     // 禁止定时器溢出中断,计数器自由运行,禁止复位,预分频系数为 16 
    // busclock/16=48Mhz/16=3000000 
    TIOS = 0x00; // 设置通道2 工作在输出比较状态,其它通道工作在输入状态 
    //TC2 = 0x2328; // 0x2328*(1/3000000)=3ms 
    //TCTL2 = 0x00; // 切断OC2 与输出引脚断开 
    // TCTL3 = 0X80; 
    TCTL4 = 0x09; 
    //TSCR1_TFFCA = 1; // 通道自动清除 
    TIE = 0x83; // 通道 0,1,2,7 中断使能 
    TSCR1_TEN = 1; // 定时器使能 
    PACTL = 0x40; //脉冲累加器使能,事件计数,下降沿计数,16 位A 累加器 
    PACNT = 0x0000; 
    TFLG1 = 0xFF; //清中断标志位
    */
    TCTL3 = 0x80; //CH7 capture on falling edges only
    //TCTL3 = 0x40; //CH7 capture on rising edges only
    //TCTL3 = 0xC0; //CH7 capture on any edge
    PACTL_PAEN = 1; //Pulse Accumulator System Enable
    PACNT = 0;
    //RTICTL = 0xAF;   // 1 010 1111   1/(16M/80*10^3)s = 5ms
    RTICTL = 0xCF;   // 1 100 1111   1/(16M/320*10^3)s = 20ms
    //RTICTL = 0xDF;   // 1 101 1111   1/(16M/800*10^3)s = 50ms
    //CRGINT_RTIE = 1;   //enable RTI Interrupt  
}

//Waiting for car start
void WaitForStart(void)
{
    int i, j;
    
//#ifdef TESTSERVO
    int servo = SERVO_ARR_MID;
//#endif
    //==== DIP_SWICH ====
    if( PORTE_PE0 == 0 )
    {
        dip1 = 1;
    }
    if( PORTE_PE1 == 0 )
    {
        dip2 = 1;
    }
    if( PORTE_PE2 == 0 )
    {
        dip3 = 1;
    }
    if( PORTE_PE3 == 0 )
    {
        dip4 = 1;
    }
    //some problem with PE4
    //if( PORTE_PE4 == 0 )
    //{
    //    dip5 = 1;
    //}
    if( PORTE_PE5 == 0 )
    {
        dip6 = 1;
        TESTSERVO = 1;
    }
    if( PORTE_PE6 == 0 )
    {
        dip7 = 1;
    }
    //PE7 -- cannot use
    //if( PORTE_PE7 == 0 )
    //{
    //    dip8 = 1;
    //}
    //===================

    while( button1pressed == 1 && button2pressed == 1 ) //wait for button pressed
    {
        button1pressed = PTT_PTT2;
        button2pressed = PTT_PTT3;
        if( button2pressed == 0 ) //debug mode
        {
            debug = 1;
        }
        slop_sensor = Read_ATD(0);
        if( slop_sensor > 105 )
        {
            BUZZER_ON();
            PORTA = 0x00;
        }
        else
        {
            BUZZER_OFF();
            PORTA = 0xFF;
        }
    }
//#ifdef TESTSERVO
if( TESTSERVO )
{
    for(;;) //test servo
    {
        button1pressed = PTT_PTT2;
        button2pressed = PTT_PTT3;
        if( button1pressed == 0 )
        {
            if( servo > RIGHTLIMIT )
            {
                servo--;
            }
        }
        else if( button2pressed == 0 )
        {
            if( servo < LEFTLIMIT )
            { 
                servo++;
            }
        }
        else;
        SetServo(servo);
        
        for( i = 1000; i > 0; i-- )
        {
            for( j = 100; j > 0; j-- )
            {
                _asm(nop);
            }
        }
    }
}
//#endif
    
    //BUZZER_ON();
    BREAK();
    LED_TURN_LEFT_ON();
    LED_TURN_RIGHT_ON();
    PORTA_PA0 = PORTE_PE7;
    PORTA_PA1 = PORTE_PE6;
    PORTA_PA2 = PORTE_PE5;
    PORTA_PA3 = PORTE_PE4;
    PORTA_PA4 = PORTE_PE3;
    PORTA_PA5 = PORTE_PE2;
    PORTA_PA6 = PORTE_PE1;
    PORTA_PA7 = PORTE_PE0;
    for( i = 10000; i > 0; i-- )
    {
        for( j = 2000; j > 0; j-- )
        {
            _asm(nop);
        }
    }
    PORTA = 0xFF;
    LED_TURN_LEFT_OFF();
    RUN();
    LED_TURN_RIGHT_OFF();
    //BUZZER_OFF();
}

// MAIN MCU_Init, include variables & modules
void MCU_Init(void)
{
    void *nouse = 0; //for memset() to return a void *
    // variables
    g_SampleFlag = 0;
    controlspeed = 0;
    app = NULL;
    CaptureCommand = 2;
    p_buff = &buff[0][0];
    SAMPLE_INTERVAL = 5;
    threshold = 0x60;
    thresholdtemp = 0x60;
    blacksum = 0;
    blackposcenter = 0;
    blackposavg = COLUMN_VALUE/2;
    blackpossum = 0;
    blackfiltered = 0;
    search_l = 0;
    search_r = 0;
    search_range = 15;
    find_start = 0;
    row_counter = 0;
    row = 0;
    RTIcounter = 0;
    pulse_counter = 0;
    turn_left = 0;
    turn_right = 0;
    servo_factor_l = 1;
    servo_factor_r = 1;
    servo_factor = 0.7;
    servo_factor_d1 = 0.1;
    servo_factor_d2 = 1;
    cam_factor = 0.8;
    get_lost_time = 0;
    image_lost = 0;
    ideal_speed = 0;
    pid_flag=0;//meiyong
    speed = 0;
    ideal_speed = 0;
    constant_speed = 500;
    prediction = 20;
    valid_prediction_value = 0;
    pre_valid = 0;
    pre_d_valid = 0;
    slope_var = 0;
    max_slope_var_pos = 0;
    leftpeak = rightpeak = 0;
    leftpeakpos = rightpeakpos = 0;
    farcurve = 0;
    farslope1 = farslope2 = 0;
    nearcurve = 0;
    nearslope1 = nearslope2 = 0;
    slope_product_near = 0;
    slope_product_far = 0;
    nearest_linecenter = previous_nearest_linecenter = COLUMN_VALUE/2;
    permit_nearest_linecenter_err = 30;
    line_dir = pre_line_dir = 0;
    permit_line_dir_err = 100;
    image_core = pre_image_core = COLUMN_VALUE/2;   //center of the whole image
    permit_image_core_err = 100;
    nearest_valid_line = -1;
    continuous_valid_line = 0;
    pre_error = 0;
    pre_d_error = 0;
    pk = 0;
    pidcounter = 0;
    isbreak = 0;
    slop_sensor = 0;
    overspeedtime = 0;
    routetype = 0;
    scheme = 0;
    counter_straight = counter_ss = counter_bs = counter_left = counter_right = counter_slope = 0;
    start_counter = 0;
    image_counter = 0;
    test = 0;
    
    ideal_angle = 0;
    cam_pre_error = 0;
    cam_pre_d_error = 0;
    cam_pk = 0;
    currentangle = 0;
    newangle = 0;
    angle = CAM_SERVO_ARR_MID;
    campos = CAM_SERVO_ARR_MID; //absolute position
    pre_campos = campos;
    
    flag_image_lost = 1;
    flag_line_dir_err = 0;
    
    flag = 0;
    findroute = 0;
    debug = 0;
    button1pressed = 1;
    button2pressed = 1;
    flag_start = 1; //set find_start when start car
    TESTSERVO = 0;
    dip1 = dip2 = dip3 = dip4 = dip5 = dip6 = dip7 = dip8 = 0;
    
    nouse = memset(buff, 0, sizeof(buff));
    nouse = memset(data_b, 0, sizeof(data_b));
    nouse = memset(Line_Center, COLUMN_VALUE/2, sizeof(Line_Center));
    nouse = memset(Line_Center_bak, COLUMN_VALUE/2, sizeof(Line_Center_bak));
    nouse = memset(edge_ll, 0, sizeof(edge_ll));
    nouse = memset(edge_rr, 0, sizeof(edge_rr));
    
    // modules
    SetBusCLK_80M();
    TimerInit();
    Port_Init();
    SCI_Init();
    TimerAccInit();
    ATD_Init();
    PWM_Init(); //Control run or not
}