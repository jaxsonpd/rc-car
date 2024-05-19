/* File:   motor_control.h
   Author: I Patel & D Hawes
   Date:   9 May 2024
   Descr:  modulisation of pwm control
*/

#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H


void motor_init(void);

void set_duty(int duty_cycle_left,int duty_cycle_right);

#endif