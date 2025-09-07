/**
 * @file    led.c
 * @brief   LED 控制驱动程序实现文件
 * @author  HuiSpec
 * @date    2025-09-01
 * @version 1.0.0
 * 
 * @details 该文件包含了 LED 的初始化、点亮、熄灭以及切换状态等功能的实现。
 *          主要功能包括初始化 LED、点亮 LED、熄灭 LED、切换 LED 状态。
 *          该驱动程序利用了 STM32 HAL 库实现了对 LED 的控制。
 * 
 * @note    使用前请确保已正确配置 LED 控制的 GPIO 引脚。
 * 
 * @copyright Copyright © 2023 HuiSpec. All rights reserved.
 */

#include "led.h"

// 初始化LED
void LED_Init(void)
{
    HAL_GPIO_WritePin(LED_PIN_GPIO_Port, LED_PIN_Pin, GPIO_PIN_SET);
}

// 点亮LED
void LED_On(void)
{
    HAL_GPIO_WritePin(LED_PIN_GPIO_Port, LED_PIN_Pin, GPIO_PIN_RESET);
}

// 熄灭LED
void LED_Off(void)
{
    HAL_GPIO_WritePin(LED_PIN_GPIO_Port, LED_PIN_Pin, GPIO_PIN_SET);
}

// 切换LED状态
void LED_Toggle(void)
{
    HAL_GPIO_TogglePin(LED_PIN_GPIO_Port, LED_PIN_Pin);
}



