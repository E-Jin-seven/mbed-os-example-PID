/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "platform/mbed_thread.h"
#define BLINKING_RATE_MS1                                                    10
#define VCC (3.3)
#define delta                                                                0.01
#define KP                                                                   0.3
#define KI                                                                   1.0
#define KD                                                                   0.03
#define target_value                                                         0.5
DigitalOut Digt0_R(D0);
DigitalOut Digt1_R(D2);
PwmOut mypwm_R(D1);

DigitalOut Digt0_L(D3);
DigitalOut Digt1_L(D5);
PwmOut mypwm_L(D4);

AnalogIn Ain1(A0);
AnalogIn Ain2(A1);

float max_min_control(float cont_val){

    if(cont_val>1){
        cont_val = 1;
    }else if(cont_val<0){
        cont_val = 0.1;
    }

    return cont_val;
}

float PID_R(float perc_R){
    
    float p = 0;
    float i = 0;
    float d = 0;
    float value[2] = {};
    float integ = 0;

    value[0] = value[1];
    value[1] = target_value - (perc_R/100);          /// 本来と逆にしてある
    integ   += (value[0]+value[1]) / 2 * delta;

    p = KP * value[1];
    i = KI * integ;
    d = KD * (value[1]-value[0]) / delta;

    return max_min_control(p+i+d);
}

float PID_L(float perc_L){
    
    float p = 0;
    float i = 0;
    float d = 0;
    float value[2] = {};
    float integ = 0;

    value[0] = value[1];
    value[1] = target_value - (perc_L/100);          /// 本来と逆にしてある
    integ   += (value[0]+value[1]) / 2 * delta;

    p = KP * value[1];
    i = KI * integ;
    d = KD * (value[1]-value[0]) / delta;

    return max_min_control(p+i+d);
}

int main()
{
    mypwm_R.period(0.01f);
    Digt0_R = 1;
    Digt1_R = 0;

    mypwm_L.period(0.01f);
    Digt0_L = 1;
    Digt1_L = 0;

    while(1){

        float norm_R = Ain1.read();
        float perc_R = norm_R * 100;
        float volt_R = norm_R * VCC;

        float norm_L = Ain2.read();
        float perc_L = norm_L * 100;
        float volt_L = norm_L * VCC;

        float cont_val_R = PID_R(perc_R);
        float cont_val_L = PID_L(perc_L);

        mypwm_R.write(cont_val_R);
        mypwm_R.write(cont_val_L);

        printf("Analog_R = %3.1f, %3.2f, %3.2f\n",perc_R,norm_R,volt_R);
        printf("Analog_L = %3.1f, %3.2f, %3.2f\n",perc_L,norm_L,volt_L);
        printf("control_R = %3.1f,control_L = %3.1f\n",cont_val_R,cont_val_L);
        thread_sleep_for(BLINKING_RATE_MS1);
    }
}
        



