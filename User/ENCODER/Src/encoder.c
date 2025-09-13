/**
 * @file    encoder.c
 * @brief   编码器电机驱动程序实现文件
 * @author  HuiSpec
 * @date    2025-09-01
 * @version 1.0.0
 *
 * @details 该文件包含了编码器电机的初始化、速度设置、方向控制以及速度计算等功能的实现。
 *          主要功能包括初始化编码器电机、设置电机速度、电机正转、电机反转、计算电机速度。
 *          该驱动程序利用了定时器和编码器接口实现了电机的精确控制。
 *
 * @note    使用前请确保已正确配置编码器和定时器接口。
 *
 * @copyright Copyright © 2023 HuiSpec. All rights reserved.
 */

#include "encoder.h"


// 全局变量存储速度
float motor_speed = 0.0f;

// 滤波器系数
float filter_coefficient = 0.3f;

PIDController pid;

/**
 * @brief 初始化编码器电机
 *
 * 该函数负责初始化与编码器电机相关的硬件资源，包括AIN端口、PWM信号生成器以及定时器。
 * AIN端口用于控制电机方向，PWM信号生成器用于控制电机速度，定时器用于编码器模式和定时记数。
 * 最后，启动TIM1的PWM模式通道1。
 */
void Encoder_Motor_Init()
{
    MX_GPIO_Init();                           // 初始化AIN端口
    MX_TIM1_Init();                           // 初始化TIM1以产生PWM信号
    MX_TIM2_Init();                           // 初始化TIM2以编码器模式运行
    MX_TIM3_Init();                           // 初始化TIM3作为定时器记数使用
    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1); // 启动TIM1的PWM模式通道1

    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_1); // 开启编码器A
    HAL_TIM_Encoder_Start(&htim3, TIM_CHANNEL_2); // 开启编码器B
    HAL_TIM_Base_Start_IT(&htim2);                // 使能定时器2中断
}
/**
 * @brief 设置编码器电机的速度和运行模式
 *
 * 该函数根据输入的模式和速度值来控制电机的运行。
 * 模式参数用于设置电机的旋转方向和制动状态，速度参数用于设置电机的转速。
 * @param mode 电机运行模式，0和1用于正反向旋转，2和3用于制动
 * @param speed 电机速度rps，范围0-FULL_SPEED_RPM，实际PWM占空比为(speed/FULL_SPEED_RPM) * 72
 */
void Encoder_Motor_SetSpeed(uint8_t mode, uint16_t speed)
{
    switch (mode)
    {
    case 0:
        // 设置电机正向旋转
        Encoder_Motor_AIN1(0);
        Encoder_Motor_AIN2(1);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (speed / FULL_SPEED_RPM) * 7200);
        break;
    case 1:
        // 设置电机反向旋转
        Encoder_Motor_AIN1(1);
        Encoder_Motor_AIN2(0);
        __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, (speed / FULL_SPEED_RPM) * 7200);
        break;
    case 2:
        // 设置电机制动状态（正向制动）
        Encoder_Motor_AIN1(1);
        Encoder_Motor_AIN2(1);
        break;
    case 3:
        // 设置电机制动状态（停止）
        Encoder_Motor_AIN1(0);
        Encoder_Motor_AIN2(0);
        break;
    default:
        break;
    }
}

/**
 * @brief 设置电机全功率正转
 * @param 无参数
 * @return 无返回值
 */
void motor_positive()
{
    // 设置电机输入引脚AIN1为低电平
    Encoder_Motor_AIN1(0);
    // 设置电机输入引脚AIN2为高电平
    Encoder_Motor_AIN2(1);
    // 设置TIM1通道1的比较值为7200，控制电机正转速度
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 7200);
}

/**
 * @brief 设置电机全功率反转
 * @param 无参数
 * @return 无返回值
 */
void motor_reverse()
{
    // 设置电机输入引脚AIN1为高电平
    Encoder_Motor_AIN1(1);
    // 设置电机输入引脚AIN2为低电平
    Encoder_Motor_AIN2(0);
    // 设置TIM1通道1的比较值为7200，控制电机反转速度
    __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, 7200);
}

/**
 * @brief 测电机满转速度
 *
 * 该函数在定时器中断中调用，用于计算当前电机满转速。
 */
void Motor_Speed()
{
    // 假设TIM2的中断频率为10Hz，即dt = 0.1秒
    float dt = 0.1f;
    // 读取TIM3的计数值
    int16_t counter_diff = (int)((int16_t)__HAL_TIM_GET_COUNTER(&htim3));

    float speed_rps =
        ((counter_diff / PULSES_PER_REVOLUTION / FREQUENCY_DOUBLING_COEFFICIENT) * (1.0f / dt)) /
        REDUCTION_RATIO; // 计算速度（转/秒）
    motor_speed =
        filter_coefficient * SPEED_TO_RPM(speed_rps) + (1.0f - filter_coefficient) * motor_speed;

    __HAL_TIM_SET_COUNTER(&htim3, 0);
}

void Update_Motor_Speed(float setpoint)
{
    // 假设TIM2的中断频率为10Hz，即dt = 0.1秒
    float dt = 0.1f;
    // 读取TIM3的计数值
    int16_t counter_diff = (int)((int16_t)__HAL_TIM_GET_COUNTER(&htim3));

    float speed_rps =
        ((counter_diff / PULSES_PER_REVOLUTION / FREQUENCY_DOUBLING_COEFFICIENT) * (1.0f / dt)) /
        REDUCTION_RATIO; // 计算速度（转/秒）

    // 使用简单的滤波器
    motor_speed =
        filter_coefficient * SPEED_TO_RPM(speed_rps) + (1.0f - filter_coefficient) * motor_speed;

    // 使用PID控制器计算新的PWM占空比
    float newPWM = pid_update(&pid, setpoint, motor_speed);

    if (newPWM >= 0)
    {
        Encoder_Motor_SetSpeed(0, (uint16_t)newPWM);
    }
    else
    {
        Encoder_Motor_SetSpeed(1, (uint16_t)fabs(newPWM));
    }

    // 设置电机速度为计算出的PWM值

    __HAL_TIM_SET_COUNTER(&htim3, 0);
}

// /**
//  * @brief TIM2 中断回调函数
//  *
//  * 该函数每100ms被调用一次，用户可以在这里添加定时任务，包括读取编码器的计数值以计算电机速度。
//  */
// void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim)
// {
//     if (htim == &htim2)
//     {
//         extern float target_speed_rps; // 引用全局目标速度
//         Update_Motor_Speed(target_speed_rps);
//     }
// }
