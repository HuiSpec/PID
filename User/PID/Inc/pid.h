#ifndef PID_H
#define PID_H

#include "stdint.h"

// PID控制器结构体定义
typedef struct
{
    float Kp;
    float Ki; // 注意：Ki 已包含采样时间缩放（或在 update 中乘 Ts）
    float Kd;
    float Ts;      // 采样周期（秒）
    float out_min; // 输出限幅下界
    float out_max; // 输出限幅上界
    // 内部状态
    float integrator;
    float prev_error;
    float prev_measurement;
    float differentiator; // 用于滤波的微分项状态
    float tau;            // 微分滤波时间常数（tau >= 0）
}PIDController;

extern PIDController pid;
// 初始化PID控制器
void pid_init(PIDController* pid, float Kp, float Ki, float Kd, float Ts, float out_min, float out_max,
              float tau);
void pid_reset(PIDController* pid);
// 计算PID控制器输出
float pid_update(PIDController* pid, float setpoint, float measurement);



#endif
