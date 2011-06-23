#include "includes.h"

void PID(void)
{
    int error,d_error,dd_error;
    error = ideal_speed - pulse_counter;    //计算偏差
    d_error = error - pre_error;    //两次偏差之差
    dd_error = d_error - pre_d_error;   //三次偏差之差

    pre_error = error;  //存储当前偏差,作为下次的参考
    pre_d_error = d_error;  //存储两次偏差之差，作为下次参考
    pk += kp*d_error + ki*error + kd*dd_error;
    if ( pk > 8000 )
    {
        pk = 8000;
    }
    else if ( pk < 0 )
    {                  
        pk = 0;
    }
    else;
    speed = pk;
}        

void SpeedControl(void)
{
    int index = 0;
    int blackposavgtemp = 0;
    int direction_err = 0;
    int cam_err = 0;
    float weight = 0;
    int i = 0;
    
    direction_err = blackposavg - COLUMN_VALUE/2;
    direction_err += campos - CAM_SERVO_ARR_MID;
    cam_err = abs(campos - CAM_SERVO_ARR_MID);
    valid_prediction_value = abs((ROW_VALUE - prediction) + (float)cam_err*0.5);
    
    if( dip1 )  //constant speed
    {
        ideal_speed = constant_speed;
    }
    
    else if( dip2 ) //stable
    {
        if( valid_prediction_value < 20 )
        {
            index = (int)( (float)(0.05*valid_prediction_value*valid_prediction_value) + 15);
            index = index > 34 ? 34 : index;
            ideal_speed = pulse_speed[index];
        }
        else
        {
            ideal_speed = constant_speed;
        }
            
        if( pulse_counter - ideal_speed > 250 )
        {
            STOP();
            overspeedtime++;
            if( overspeedtime >= 3 )
            {
                BREAK();
            }
        }
        else
        {
            overspeedtime = 0;
        }
    }
        
    else if( dip3 ) //normal
    {
        if( valid_prediction_value < 25 )
        {
            if( valid_prediction_value < 10 )
            {
                index = (int)( (float)(0.1*valid_prediction_value*valid_prediction_value) + 10);
            }
            else
            {
                if( valid_prediction_value < pre_valid )
                {
                    index = (int)( (float)(0.09*(valid_prediction_value - 10)*(valid_prediction_value - 10)) + 20);
                }
                else
                {
                    index = (int)( (float)(0.06*(valid_prediction_value - 10)*(valid_prediction_value - 10)) + 20);
                }
            }
            index = index > 39 ? 39 : index;
            ideal_speed = pulse_speed[index];
        }
        else
        {
            ideal_speed = constant_speed;
        }
            
        if( pulse_counter - ideal_speed > 250 )
        {
            STOP();
            overspeedtime++;
            if( overspeedtime >= 3 )
            {
                BREAK();
            }
        }
        else
        {
            overspeedtime = 0;
        }
    }
    
    else if( dip4 ) //high speed
    {
        if( valid_prediction_value < 25 )
        {
            if( valid_prediction_value < 10 )
            {
                index = (int)( (float)(0.15*valid_prediction_value*valid_prediction_value) + 1);
            }
            else
            {
                if( valid_prediction_value < pre_valid )
                {
                    index = (int)( (float)(0.2*(valid_prediction_value - 10)*(valid_prediction_value - 10)) + 15);
                }
                else
                {
                    index = (int)( (float)(0.09*(valid_prediction_value - 10)*(valid_prediction_value - 10)) + 15);
                }
            }
            index = index > 39 ? 39 : index;
            ideal_speed = pulse_speed[index];
        }
        else
        {
            ideal_speed = constant_speed;
        }
            
        if( pulse_counter - ideal_speed > 250 )
        {
            STOP();
            overspeedtime++;
            if( overspeedtime >= 3 )
            {
                BREAK();
            }
        }
        else
        {
            overspeedtime = 0;
        }
    }
    
    else
    {
        ideal_speed = 0;
    }
    
    pre_valid = valid_prediction_value;
    
    /***************************
    *       PID Control        *
    ***************************/
    if( ideal_speed - pulse_counter > 60 || ideal_speed - pulse_counter < -60 )
    {
        if( pulse_counter < ideal_speed )
        {
            RUN();
        }
        PID();
    }
    PWMDTY67 = speed;
    
    //imergent break down
    if( prediction < 10 )
    {
        image_lost += 1;
        if( image_lost >= 30 )
        {
            STOP();
        }
    }
    else
    {
        image_lost = 0; 
    }
}