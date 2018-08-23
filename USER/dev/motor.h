#ifndef __DEV_MOTOR_H_
#define __DEV_MOTOR_H_
#include "sys.h"
#include <stdio.h>

class MotorClass
{
    public:
        MotorClass();
        void init(void);
        void start(void);
        void stop(void);
};


extern MotorClass motor;
extern void motor_init(void);
extern void motor_task(void);
#endif
