/**
 * @file    pid.c
 * @brief   简单且实用的 PID 控制器实现文件
 * @author  HuiSpec
 * @date    2023-10-01
 * @version 1.0.0
 * 
 * @details 该文件包含了简单且实用的 PID 控制器的实现。
 *          该 PID 控制器支持浮点版本，适合 STM32/Arduino 等具有浮点运算能力的微控制器。
 *          主要功能包括 PID 初始化、重置和更新。
 * 
 * @note    使用前请确保已正确配置 PID 控制器的参数。
 * 
 * @copyright Copyright © 2023 HuiSpec. All rights reserved.
 */

#include "pid.h"
#include <stdint.h>

#include "pid.h"
#include <stdint.h>

/* 初始化 PID */
void pid_init(PIDController* pid, float Kp, float Ki, float Kd, float Ts, float out_min,
              float out_max, float tau)
{
    pid->Kp               = Kp;
    pid->Ki               = Ki;
    pid->Kd               = Kd;
    pid->Ts               = Ts;
    pid->out_min          = out_min;
    pid->out_max          = out_max;
    pid->integrator       = 0.0f;
    pid->prev_error       = 0.0f;
    pid->prev_measurement = 0.0f;
    pid->differentiator   = 0.0f;
    pid->tau              = tau; // 推荐 tau 在 0.01*Ts 到 10*Ts 之间尝试
}

void pid_reset(PIDController* pid)
{
    pid->integrator       = 0.0f;
    pid->prev_error       = 0.0f;
    pid->prev_measurement = 0.0f;
    pid->differentiator   = 0.0f;
}
/* 限幅辅助 */
static float clampf(float v, float lo, float hi)
{
    if (v < lo)
        return lo;
    if (v > hi)
        return hi;
    return v;
}
/* PID 核心更新：传入设定值和测量值，返回控制量 */
float pid_update(PIDController* pid, float setpoint, float measurement)
{
    float error = setpoint - measurement;
    // 比例项
    float P = pid->Kp * error;
    // 积分项（矩形积分）
    pid->integrator += 0.5f * pid->Ki * pid->Ts * (error + pid->prev_error);
    // 积分防风（限制积分值，避免积分累积过大）
    // 可将积分范围设为输出范围的一部分，或单独配置
    float integ_min = pid->out_min;
    float integ_max = pid->out_max;
    pid->integrator = clampf(pid->integrator, integ_min, integ_max);
    float I         = pid->integrator;
    // 微分项（使用测量值微分 + 一阶低通滤波，减少噪声放大）
    // differentiator 状态使用滤波器： D = ( -Kd * (measurement - prev_measurement) * (1/Ts) )
    // filtered 使用标准形式： differentiator = (2*tau - Ts)/(2*tau + Ts) * differentiator_prev
    //    - 2*Kd/(2*tau + Ts) * (measurement - prev_measurement)
    if (pid->tau <= 0.0f)
    {
        // 没有滤波，简单差分
        pid->differentiator = pid->Kd * (error - pid->prev_error) / pid->Ts;
    }
    else
    {
        float alpha        = (2.0f * pid->tau - pid->Ts) / (2.0f * pid->tau + pid->Ts);
        float diff_measure = (measurement - pid->prev_measurement) / pid->Ts;
        // 用 measurement 上的微分能减少 setpoint step 导致的 D 爆炸（常见做法）
        pid->differentiator = alpha * pid->differentiator -
                              (2.0f * pid->Kd / (2.0f * pid->tau + pid->Ts)) * diff_measure;
    }
    float D = pid->differentiator;
    // 合并输出并限幅
    float output         = P + I + D;
    float output_clamped = clampf(output, pid->out_min, pid->out_max);
    // 抗积分风（simple back-calculation）：如果输出被限幅，减小积分（选项）
    // 若输出 被限幅 且 与未限幅输出有差异，则按差异调整积分（更稳定）
    // 下面是一个简单的实现：当限幅时，撤销刚才的积分累加量
    if (output != output_clamped)
    {
        // 取消本次积分（另一种方法是使用反向补偿 gain）
        pid->integrator -= 0.5f * pid->Ki * pid->Ts * (error + pid->prev_error);
        I = pid->integrator;
        // 可考虑更复杂的反向补偿法： integrator += (output_clamped - output) * K_aw
    }
    // 更新历史值
    pid->prev_error       = error;
    pid->prev_measurement = measurement;
    return output_clamped;
}