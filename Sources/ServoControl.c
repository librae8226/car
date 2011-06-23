#include "includes.h"

void SetServo(int angle)
{
    //Protect servo
    if( angle < RIGHTLIMIT )
    {
        angle = RIGHTLIMIT;
    }
    else if( angle > LEFTLIMIT )
    {
        angle = LEFTLIMIT;
    }
    else;
    SetPWM01( 20000, (unsigned int)servo_arr[angle] );
}

void SetCam(int angle)
{
    //Protect servo
    if( angle < CAM_RIGHTLIMIT )
    {
        angle = CAM_RIGHTLIMIT;
    }
    else if( angle > CAM_LEFTLIMIT )
    {
        angle = CAM_LEFTLIMIT;
    }
    else;
    SetPWM45( 20000, (unsigned int)servo_arr[angle] );
}

void SetPWM01(unsigned int period, unsigned int duty) // Control PWM01,16 bits
{
    PWMPER01 = period;
    PWMDTY01 = duty;
}

void SetPWM45(unsigned int period, unsigned int duty) // Control PWM45,16 bits
{
    PWMPER45 = period;
    PWMDTY45 = duty;
}

void AutoControl(void)
{   
    float direction_err = 0;
    float k = 0;
    float pos_err = 0;
    steer = MIDDLE;
    
    //calculate route direction
    direction_err = (blackposavg - COLUMN_VALUE/2) + (campos - CAM_SERVO_ARR_MID);
    
    if( prediction < 20 )
    {
        if( prediction != 0 )
        {
            k = (Line_Center[ROW_VALUE - 1 - prediction] - Line_Center[39])/(prediction);
        }
        else
        {
            k = 100;
        }
        pos_err = (Line_Center[ROW_VALUE - 1 - prediction] - Line_Center[39]) + (campos - pre_campos);
    }
    else
    {
        k = (Line_Center[19] - Line_Center[39])/20;
        pos_err = (Line_Center[19] - Line_Center[39]) + (campos - pre_campos);
    }
    
    pre_campos = campos;
    //SetServo( (int)( SERVO_ARR_MID - (float)( servo_factor*direction_err) ) );    //P only
    SetServo( (int)( SERVO_ARR_MID - (float)( servo_factor*direction_err + servo_factor_d1 * pos_err) ) );    //P, D1
    //SetServo( (int)( SERVO_ARR_MID - (float)( servo_factor*direction_err + servo_factor_d2 * k) ) );  //P, D2
    //SetServo( (int)( SERVO_ARR_MID - (float)( servo_factor*direction_err + servo_factor_d1 * pos_err + servo_factor_d2 * k) ) );  //P, D1, D2
    //test = (int)( SERVO_ARR_MID - (float)( servo_factor*direction_err) );
}

void CamControl(void)
{
    float weight = 0;   
    float posavg = 0;
    int i = 0;
    int k = 0;
    int ADtemp = 0;
    int cam_err = 0;
    
    cam_err = abs(campos - CAM_SERVO_ARR_MID);
    
    for( i = ROW_VALUE - 1; i >= 0; i-- )
    {
        if( i < 5 )
        {
            weight = 0;
        }
        else if( i >= 5 && i < 10 )
        {
            weight = 0.01;
        }
        else if( i >= 10 && i < 15 )
        {
            weight = 0.02;
        }
        else if( i >= 15 && i < 20 )
        {
            weight = 0.03;
        }
        else if( i >= 20 && i < 25 )
        {
            weight = 0.04;
        }
        else if( i >= 25 && i < 30 )
        {
            weight = 0.04;
        }
        else if( i >= 30 && i < 35 )
        {
            weight = 0.04;
        }
        else
        {
            weight = 0.02;
        }
        //weight = GetWeight(i, scheme);
        posavg += Line_Center[i] * weight;
    }
    
    ideal_angle = (int)(posavg + 0.5) - COLUMN_VALUE/2;
    
    if( prediction < 20 )
    {
        ideal_angle = Line_Center[ROW_VALUE - 1 - prediction] - COLUMN_VALUE/2;
    }
    else
    {
        ideal_angle = (int)(posavg + 0.5) - COLUMN_VALUE/2;
    }
    
    if( prediction > 30 )
    {
        if( ideal_angle > 10 && ideal_angle < 20 )
        {
            angle -= 1;
        }
        else if( ideal_angle > 20 && ideal_angle < 30 )
        {
            angle -= 1;
        }
        else if( ideal_angle > 30 )
        {
            angle -= 1;
        }
        else if( ideal_angle < -10 && ideal_angle > -20 )
        {
            angle += 1;
        }
        else if( ideal_angle < -20 && ideal_angle > -30 )
        {
            angle += 1;
        }
        else if( ideal_angle < -30 )
        {
            angle += 1;
        }
        else;
    }
    else
    {
        if( ideal_angle > 10 && ideal_angle < 20 )
        {
            angle -= 1;
        }
        else if( ideal_angle > 20 && ideal_angle < 30 )
        {
            angle -= 5;
        }
        else if( ideal_angle > 30 )
        {
            angle -= 10;
        }
        else if( ideal_angle < -10 && ideal_angle > -20 )
        {
            angle += 1;
        }
        else if( ideal_angle < -20 && ideal_angle > -30 )
        {
            angle += 5;
        }
        else if( ideal_angle < -30 )
        {
            angle += 10;
        }
        else;
    }
    
    //IMPORTANT
    if( angle < CAM_RIGHTLIMIT )
    {
        angle = CAM_RIGHTLIMIT;
    }
    else if( angle > CAM_LEFTLIMIT )
    {
        angle = CAM_LEFTLIMIT;
    }
    else;
    
    SetCam(angle);
    
    ADtemp = (Read_ATD(2) + Read_ATD(2) + Read_ATD(2) + Read_ATD(2) + Read_ATD(2))/5;
    campos =  CAM_SERVO_ARR_MID + CAM_SERVO_MID_AD - ADtemp;   //absolute position
}

void Cam_PID(void)
{
    int error,d_error,dd_error;
    error = ideal_angle;    //计算偏差
    d_error = error - cam_pre_error;    //两次偏差之差
    dd_error = d_error - cam_pre_d_error;   //三次偏差之差

    cam_pre_error = error;  //存储当前偏差,作为下次的参考
    cam_pre_d_error = d_error;  //存储两次偏差之差，作为下次参考
    cam_pk += kp_cam*d_error + ki_cam*error + kd_cam*dd_error;
    if ( cam_pk > CAM_LEFTLIMIT )
    {
        cam_pk = CAM_RIGHTLIMIT;
    }
    else if ( cam_pk < CAM_RIGHTLIMIT )
    {                  
        cam_pk = CAM_RIGHTLIMIT;
    }
    else;
    newangle = cam_pk;
}