/*
 * Copyright (c) 2016 Zibin Zheng <znbin@qq.com>
 * All rights reserved
 */

#include "multi_button.h"

// 宏定义：回调函数执行并进行空指针检查
#define EVENT_CB(ev)                                                                               \
    do                                                                                             \
    {                                                                                              \
        if (handle->cb[ev])                                                                        \
            handle->cb[ev](handle);                                                                \
    } while (0)

// 按钮句柄链表头部
static Button* head_handle = NULL;

// 前向声明
static void button_handler(Button* handle);
static inline uint8_t button_read_level(Button* handle);

/**
 * @brief  初始化按钮结构体句柄
 * @param  handle: 按钮句柄结构体
 * @param  pin_level: 读取连接按钮的HAL GPIO电平的函数
 * @param  active_level: 按下时的GPIO电平
 * @param  button_id: 按钮标识符
 * @retval None
 */
void button_init(Button* handle, uint8_t (*pin_level)(uint8_t), uint8_t active_level,
                 uint8_t button_id)
{
    if (!handle || !pin_level)
        return; // 参数验证

    memset(handle, 0, sizeof(Button));
    handle->event            = (uint8_t)BTN_NONE_PRESS;
    handle->hal_button_level = pin_level;
    handle->button_level     = !active_level; // 初始化为活动电平的反值
    handle->active_level     = active_level;
    handle->button_id        = button_id;
    handle->state            = BTN_STATE_IDLE;
}

/**
 * @brief  附加按钮事件回调函数
 * @param  handle: 按钮句柄结构体
 * @param  event: 触发事件类型
 * @param  cb: 回调函数
 * @retval None
 */
void button_attach(Button* handle, ButtonEvent event, BtnCallback cb)
{
    if (!handle || event >= BTN_EVENT_COUNT)
        return; // 参数验证
    handle->cb[event] = cb;
}

/**
 * @brief  分离按钮事件回调函数
 * @param  handle: 按钮句柄结构体
 * @param  event: 触发事件类型
 * @retval None
 */
void button_detach(Button* handle, ButtonEvent event)
{
    if (!handle || event >= BTN_EVENT_COUNT)
        return; // 参数验证
    handle->cb[event] = NULL;
}

/**
 * @brief  获取发生的按钮事件
 * @param  handle: 按钮句柄结构体
 * @retval 按钮事件
 */
ButtonEvent button_get_event(Button* handle)
{
    if (!handle)
        return BTN_NONE_PRESS;
    return (ButtonEvent)(handle->event);
}

/**
 * @brief  获取按钮按压的重复次数
 * @param  handle: 按钮句柄结构体
 * @retval 重复次数
 */
uint8_t button_get_repeat_count(Button* handle)
{
    if (!handle)
        return 0;
    return handle->repeat;
}

/**
 * @brief  重置按钮状态为空闲状态
 * @param  handle: 按钮句柄结构体
 * @retval None
 */
void button_reset(Button* handle)
{
    if (!handle)
        return;
    handle->state        = BTN_STATE_IDLE;
    handle->ticks        = 0;
    handle->repeat       = 0;
    handle->event        = (uint8_t)BTN_NONE_PRESS;
    handle->debounce_cnt = 0;
}

/**
 * @brief  检查按钮是否当前被按下
 * @param  handle: 按钮句柄结构体
 * @retval 1: 按下, 0: 未按下, -1: 错误
 */
int button_is_pressed(Button* handle)
{
    if (!handle)
        return -1;
    return (handle->button_level == handle->active_level) ? 1 : 0;
}

/**
 * @brief  读取按钮电平，并进行内联优化
 * @param  handle: 按钮句柄结构体
 * @retval 按钮电平
 */
static inline uint8_t button_read_level(Button* handle)
{
    return handle->hal_button_level(handle->button_id);
}

/**
 * @brief  按钮驱动核心函数，驱动状态机
 * @param  handle: 按钮句柄结构体
 * @retval None
 */
static void button_handler(Button* handle)
{
    uint8_t read_gpio_level = button_read_level(handle);

    // 当不在空闲状态时，增加计数器
    if (handle->state > BTN_STATE_IDLE)
    {
        handle->ticks++;
    }

    /*------------消抖处理---------------*/
    if (read_gpio_level != handle->button_level)
    {
        // 继续读取相同的电平以进行消抖
        if (++(handle->debounce_cnt) >= DEBOUNCE_TICKS)
        {
            handle->button_level = read_gpio_level;
            handle->debounce_cnt = 0;
        }
    }
    else
    {
        // 电平未变化，重置计数器
        handle->debounce_cnt = 0;
    }

    /*-----------------状态机-------------------*/
    switch (handle->state)
    {
    case BTN_STATE_IDLE:
        if (handle->button_level == handle->active_level)
        {
            // 按钮按下检测到
            handle->event = (uint8_t)BTN_PRESS_DOWN;
            EVENT_CB(BTN_PRESS_DOWN);
            handle->ticks  = 0;
            handle->repeat = 1;
            handle->state  = BTN_STATE_PRESS;
        }
        else
        {
            handle->event = (uint8_t)BTN_NONE_PRESS;
        }
        break;

    case BTN_STATE_PRESS:
        if (handle->button_level != handle->active_level)
        {
            // 按钮释放
            handle->event = (uint8_t)BTN_PRESS_UP;
            EVENT_CB(BTN_PRESS_UP);
            handle->ticks = 0;
            handle->state = BTN_STATE_RELEASE;
        }
        else if (handle->ticks > LONG_TICKS)
        {
            // 长按检测到
            handle->event = (uint8_t)BTN_LONG_PRESS_START;
            EVENT_CB(BTN_LONG_PRESS_START);
            handle->state = BTN_STATE_LONG_HOLD;
        }
        break;

    case BTN_STATE_RELEASE:
        if (handle->button_level == handle->active_level)
        {
            // 按钮再次按下
            handle->event = (uint8_t)BTN_PRESS_DOWN;
            EVENT_CB(BTN_PRESS_DOWN);
            if (handle->repeat < PRESS_REPEAT_MAX_NUM)
            {
                handle->repeat++;
            }
            EVENT_CB(BTN_PRESS_REPEAT);
            handle->ticks = 0;
            handle->state = BTN_STATE_REPEAT;
        }
        else if (handle->ticks > SHORT_TICKS)
        {
            // 超时达到，确定点击类型
            if (handle->repeat == 1)
            {
                handle->event = (uint8_t)BTN_SINGLE_CLICK;
                EVENT_CB(BTN_SINGLE_CLICK);
            }
            else if (handle->repeat == 2)
            {
                handle->event = (uint8_t)BTN_DOUBLE_CLICK;
                EVENT_CB(BTN_DOUBLE_CLICK);
            }
            handle->state = BTN_STATE_IDLE;
        }
        break;

    case BTN_STATE_REPEAT:
        if (handle->button_level != handle->active_level)
        {
            // 按钮释放
            handle->event = (uint8_t)BTN_PRESS_UP;
            EVENT_CB(BTN_PRESS_UP);
            if (handle->ticks < SHORT_TICKS)
            {
                handle->ticks = 0;
                handle->state = BTN_STATE_RELEASE; // 继续等待更多按压
            }
            else
            {
                handle->state = BTN_STATE_IDLE; // 序列结束
            }
        }
        else if (handle->ticks > SHORT_TICKS)
        {
            // 按住时间过长，视为正常按下
            handle->state = BTN_STATE_PRESS;
        }
        break;

    case BTN_STATE_LONG_HOLD:
        if (handle->button_level == handle->active_level)
        {
            // 继续按住
            handle->event = (uint8_t)BTN_LONG_PRESS_HOLD;
            EVENT_CB(BTN_LONG_PRESS_HOLD);
        }
        else
        {
            // 长按释放
            handle->event = (uint8_t)BTN_PRESS_UP;
            EVENT_CB(BTN_PRESS_UP);
            handle->state = BTN_STATE_IDLE;
        }
        break;

    default:
        // 无效状态，重置为空闲状态
        handle->state = BTN_STATE_IDLE;
        break;
    }
}

/**
 * @brief  启动按钮工作，将句柄添加到工作列表
 * @param  handle: 目标句柄结构体
 * @retval 0: 成功, -1: 已存在, -2: 无效参数
 */
int button_start(Button* handle)
{
    if (!handle)
        return -2; // 无效参数

    Button* target = head_handle;
    while (target)
    {
        if (target == handle)
            return -1; // 已存在
        target = target->next;
    }

    handle->next = head_handle;
    head_handle  = handle;
    return 0;
}

/**
 * @brief  停止按钮工作，将句柄从工作列表中移除
 * @param  handle: 目标句柄结构体
 * @retval None
 */
void button_stop(Button* handle)
{
    if (!handle)
        return; // 参数验证

    Button** curr;
    for (curr = &head_handle; *curr;)
    {
        Button* entry = *curr;
        if (entry == handle)
        {
            *curr       = entry->next;
            entry->next = NULL; // 清除下一个指针
            return;
        }
        else
        {
            curr = &entry->next;
        }
    }
}

/**
 * @brief  背景计时，定时器重复调用间隔5ms
 * @param  None
 * @retval None
 */
void button_ticks(void)
{
    Button* target;
    for (target = head_handle; target; target = target->next)
    {
        button_handler(target);
    }
}
