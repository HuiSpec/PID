// button.h
#ifndef _BUTTON_H_
#define _BUTTON_H_

#include "stm32f1xx_hal.h"
#include "multi_button.h"
#include "main.h"
#include <stdio.h>

#define BUTTON_UP 0x40
#define BUTTON_DOWN 0x20
#define BUTTON_LEFT 0x10
#define BUTTON_RIGHT 0x08
#define BUTTON_MID 0x04
#define BUTTON_SET 0x02
#define BUTTON_RST 0x01

// 前向声明
void button1_single_click_callback(Button* btn_handle);
void button2_single_click_callback(Button* btn_handle);
void button3_single_click_callback(Button* btn_handle);
void button4_single_click_callback(Button* btn_handle);
void button5_single_click_callback(Button* btn_handle);
void button6_single_click_callback(Button* btn_handle);
void button7_single_click_callback(Button* btn_handle);

extern uint8_t button_status;

// 初始化函数声明
void init_buttons(void);

#endif // _BUTTON_H_
