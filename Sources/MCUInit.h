#ifndef __MCUINIT_H__
#define __MCUINIT_H__

extern Bool dip0, dip1, dip2, dip3, dip4, dip5, dip6, dip7, dip8;

void WaitForStart(void);
void MCU_Init(void); //variables & modules initialize

/*Set PLL*/
void SetBusCLK_80M(void);
void SetBusCLK_48M(void);

/*ATD*/
void ATD_Init_Original(void);
void ATD_Init(void);
//unsigned char Read_ATD(void);
unsigned char Read_ATD(unsigned char ch);

/*IRQ*/
void IRQ_Init(void);

/*Port*/
void Port_Init(void);

/*PWM*/
void PWM_Init(void);

/*SCI*/
void SCI_Init(void);
//void SCI_Transmit(unsigned char t_data);
//unsigned char SCI_Recieve(void);
void uart_putchar(unsigned char ch);

/*TIM*/
//void InputCapture_Init(void);
//void OutputCompare_Init(void);
//void PIT_Init(void);
void TimerInit(void);
void TimerAccInit(void);

#endif