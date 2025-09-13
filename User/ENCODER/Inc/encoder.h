#ifndef __ENCODER_H
#define __ENCODER_H

#include "gpio.h"
#include "main.h"
#include "pid.h"
#include "tim.h"
#include <math.h>

// 编码器分辨率（每转一圈的脉冲数）
#define PULSES_PER_REVOLUTION 11.0f
// 分频系数
#define FREQUENCY_DOUBLING_COEFFICIENT 4
// 减速比
#define REDUCTION_RATIO 4.4
// 满转速（单位为转/秒，可根据实际需求调整）
#define FULL_SPEED_RPM 1300.0f // 假设满转速为 1000 rps

#define SPEED_UNIT_IS_RPM

#ifndef SPEED_UNIT_IS_RPM
#define SPEED_UNIT_IS_RPS // 默认为 rps
#else
#define SPEED_UNIT_IS_RPS 0
#endif

// 单位转换宏
#if defined(SPEED_UNIT_IS_RPM)
#define SPEED_TO_RPM(x) ((x)*60.0f)
#else
#define SPEED_TO_RPM(x) (x)
#endif

#define Encoder_Motor_AIN1(x)                                                                      \
    ((x == 1) ? (ENCODER_AIN1_GPIO_Port->BSRR = ENCODER_AIN1_Pin)                                  \
              : (ENCODER_AIN1_GPIO_Port->BRR = ENCODER_AIN1_Pin))
#define Encoder_Motor_AIN2(x)                                                                      \
    ((x == 1) ? (ENCODER_AIN2_GPIO_Port->BSRR = ENCODER_AIN2_Pin)                                  \
              : (ENCODER_AIN2_GPIO_Port->BRR = ENCODER_AIN2_Pin))

extern float motor_speed_rps;
extern PIDController pid;


void Encoder_Motor_Init();
void Encoder_Motor_SetSpeed(uint8_t mode, uint16_t speed);
void Motor_Speed();
void Update_Motor_Speed(float setpoint);
void motor_positive();
void motor_reverse();

#endif
