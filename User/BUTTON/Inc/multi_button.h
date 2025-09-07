/*
 * Copyright (c) 2016 Zibin Zheng <znbin@qq.com>
 * All rights reserved
 */

#ifndef _MULTI_BUTTON_H_
#define _MULTI_BUTTON_H_

#include <stdint.h>
#include <string.h>

// 配置常量 - 根据需要可以修改
#define TICKS_INTERVAL 5                   // ms - 定时器中断间隔
#define DEBOUNCE_TICKS 3                   // MAX 7 (0 ~ 7) - 消抖滤波深度
#define SHORT_TICKS (200 / TICKS_INTERVAL) // 单击阈值
#define LONG_TICKS (1000 / TICKS_INTERVAL) // 长按阈值
#define PRESS_REPEAT_MAX_NUM 15            // 最大重复计数器值

// 前向声明
typedef struct _Button Button;

// 按钮回调函数类型
typedef void (*BtnCallback)(Button* btn_handle);

// 按钮事件类型
typedef enum
{
    BTN_PRESS_DOWN = 0,   // 按钮按下
    BTN_PRESS_UP,         // 按钮释放
    BTN_PRESS_REPEAT,     // 检测到重复按下
    BTN_SINGLE_CLICK,     // 单击完成
    BTN_DOUBLE_CLICK,     // 双击完成
    BTN_LONG_PRESS_START, // 长按开始
    BTN_LONG_PRESS_HOLD,  // 长按持续中
    BTN_EVENT_COUNT,      // 事件总数
    BTN_NONE_PRESS        // 无事件
} ButtonEvent;

// 按钮状态机状态
typedef enum
{
    BTN_STATE_IDLE = 0, // 空闲状态
    BTN_STATE_PRESS,    // 按下状态
    BTN_STATE_RELEASE,  // 释放状态等待超时
    BTN_STATE_REPEAT,   // 重复按下状态
    BTN_STATE_LONG_HOLD // 长按持续状态
} ButtonState;

// 按钮结构体
struct _Button
{
    uint16_t ticks;                                 // 计时器计数器
    uint8_t repeat : 4;                             // 重复计数器 (0-15)
    uint8_t event : 4;                              // 当前事件 (0-15)
    uint8_t state : 3;                              // 状态机状态 (0-7)
    uint8_t debounce_cnt : 3;                       // 消抖计数器 (0-7)
    uint8_t active_level : 1;                       // 活动GPIO电平 (0 或 1)
    uint8_t button_level : 1;                       // 当前按钮电平
    uint8_t button_id;                              // 按钮标识符
    uint8_t (*hal_button_level)(uint8_t button_id); // HAL函数读取GPIO
    BtnCallback cb[BTN_EVENT_COUNT];                // 回调函数数组
    Button* next;                                   // 链表中的下一个按钮
};

#ifdef __cplusplus
extern "C"
{
#endif

    // 公共API函数
    void button_init(Button* handle, uint8_t (*pin_level)(uint8_t), uint8_t active_level,
                     uint8_t button_id);
    void button_attach(Button* handle, ButtonEvent event, BtnCallback cb);
    void button_detach(Button* handle, ButtonEvent event);
    ButtonEvent button_get_event(Button* handle);
    int button_start(Button* handle);
    void button_stop(Button* handle);
    void button_ticks(void);

    // 工具函数
    uint8_t button_get_repeat_count(Button* handle);
    void button_reset(Button* handle);
    int button_is_pressed(Button* handle);

#ifdef __cplusplus
}
#endif

#endif
