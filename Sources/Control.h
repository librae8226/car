#ifndef __CONTROL_H__
#define __CONTROL_H__

#define SERVO_MID           servo_arr[61]
#define SERVO_ARR_MID       61
#define LEFT                -1
#define RIGHT               1
#define MIDDLE              0
#define RIGHTLIMIT          28
#define LEFTLIMIT           94
#define CAM_SERVO_MID       servo_arr[67]
#define CAM_SERVO_ARR_MID   67
#define CAM_LEFTLIMIT       97//117
#define CAM_RIGHTLIMIT      37//17
#define CAM_SERVO_MID_AD    96  //越大靠近赛道右侧跑
#define RUN()               PWME_PWME3 = 0;PWME_PWME7 = 1;PTM_PTM0 = 1;PTM_PTM3 = 1
#define BREAK()             PWME_PWME7 = 0;PWME_PWME3 = 1;PTM_PTM0 = 0;PTM_PTM3 = 0
#define STOP()              PWME_PWME7 = 0;PWME_PWME3 = 0

#define BUZZER_ON()             PTT_PTT6 = 0
#define BUZZER_OFF()            PTT_PTT6 = 1
#define LED_TURN_LEFT_ON()      PTM_PTM1 = 0
#define LED_TURN_LEFT_OFF()     PTM_PTM1 = 1
#define LED_TURN_RIGHT_ON()     PTM_PTM2 = 0
#define LED_TURN_RIGHT_OFF()    PTM_PTM2 = 1

#define kp                  8   //6
#define ki                  2   //2
#define kd                  0   //0

#define kp_cam              0.5   //1.5
#define ki_cam              0.1   //0
#define kd_cam              0   //0

extern unsigned char slop_sensor;   //加速度传感器值
extern int constant_speed;  //匀速值_调试用
extern byte pid_flag;   //弯度大小标志，用来判断是否pid
extern unsigned int speed;
extern unsigned int ideal_speed;    //给定的速度值
extern int pre_error;    //速度PID 前一次的速度误差值
extern int pre_d_error;  //速度PID 前一次的速度误差之差
extern int pk;           //给的实际速度，由PID计算得出

extern int ideal_angle;    //给定的偏移量
extern int cam_pre_error;    //偏移量PID 前一次的偏移量误差值
extern int cam_pre_d_error;  //偏移量PID 前一次的偏移量误差之差
extern int cam_pk;           //给的实际偏移量，由PID计算得出
extern int currentangle;   //摄像头目前的偏移量
extern int newangle;    //计算出的新偏移量
extern int angle;
extern int campos;  //data base AD, (17) small -- turnlefrt | large -- turn right (117, actually 122)
extern int pre_campos;
extern int test;    //for watching in debug


extern unsigned int RTIcounter;
extern unsigned int pulse_counter;
extern unsigned char turn_left, turn_right;
extern float servo_factor_l, servo_factor_r;
extern float servo_factor;  //when left and right is the same
extern float servo_factor_d2;
extern float servo_factor_d1;
extern float cam_factor;
extern const int servo_arr[121]; // Servo angle table
extern const int pulse_speed[40];
extern int steer;
extern unsigned int pidcounter;
extern int isbreak;
extern int overspeedtime;   //count speed exceeding time, for BREAK

volatile extern Bool debug, button1pressed, button2pressed;
volatile extern Bool TESTSERVO;
volatile extern Bool controlspeed;

void SetServo(int angle);

void SetCam(int angle);

void SetPWM01(unsigned int period, unsigned int duty); // Control PWM01,16 bits

void SetPWM45(unsigned int period, unsigned int duty); // Control PWM45,16 bits

void AutoControl(void);

void CamControl(void);

void SpeedControl(void);

void PID(void);

void Cam_PID(void);

#endif