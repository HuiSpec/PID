/**
 * @file    my_gui.c
 * @brief   OLED屏幕显示和按钮处理驱动程序实现文件
 * @author  HuiSpec
 * @date    2025-09-01
 * @version 1.0.0
 *
 * @details 该文件包含了在OLED屏幕上绘制图像和文本、处理按钮输入以及控制不同功能的实现。
 *          主要功能包括显示logo、主菜单、LED控制、电机速度控制、按钮界面和PID参数调整。
 *          该驱动程序利用了OLED库和按钮库实现了与OLED屏幕和按钮的交互。
 *
 * @note    使用前请确保已正确配置OLED屏幕和按钮接口。
 *
 * @copyright Copyright © 2023 HuiSpec. All rights reserved.
 */

#include "my_gui.h"

/**
 * @brief 在OLED屏幕上绘制logo图像并等待特定按钮按下
 * @param 无参数
 * @return 如果中间按钮按下则返回1，否则持续等待
 */
int logo()
{
    OLED_NewFrame();
    OLED_DrawImage(20, 8, &logoImg, OLED_COLOR_NORMAL);
    OLED_ShowFrame();
    while (1)
    {
        if (button_status == BUTTON_MID)
        {
            button_status = 0;
            return 1;
        }
        HAL_Delay(10);
    }
}

/**
 * @brief 显示主菜单并处理按钮输入以选择不同的菜单项
 * @param up_menu 初始选中的菜单项索引（0-3）
 * @return 根据按钮输入返回不同的菜单项索引（1-4），或0表示重置
 */
int main_menu(int up_menu)
{
    int flag = up_menu; // 初始化选中的菜单项索引

    OLED_NewFrame();
    OLED_PrintString(16, 0, "测试", &font16x16, OLED_COLOR_NORMAL);
    OLED_PrintString(16, 16, "PID", &font16x16, OLED_COLOR_NORMAL);
    OLED_PrintString(16, 32, "MPU", &font16x16, OLED_COLOR_NORMAL);

    while (1) // 无限循环，等待按钮输入
    {
        if (button_status == BUTTON_DOWN) // 检测向下按钮
        {
            button_status = 0; // 重置按钮状态
            flag++;            // 移动到下一个菜单项
            if (flag > 3)      // 如果超过最大菜单项索引
            {
                flag = 0; // 回到第一个菜单项
            }
        }
        if (button_status == BUTTON_UP) // 检测向上按钮
        {
            button_status = 0; // 重置按钮状态
            flag--;            // 移动到上一个菜单项
            if (flag < 0)      // 如果小于最小菜单项索引
            {
                flag = 3; // 回到最后一个菜单项
            }
        }
        if (button_status == BUTTON_MID) // 检测中间按钮
        {
            button_status = 0; // 重置按钮状态
            return (flag + 1); // 返回选中的菜单项索引（1-4）
        }
        if (button_status == BUTTON_RST) // 检测重置按钮
        {
            return 0; // 返回0表示重置
        }

        switch (flag) // 根据当前选中的菜单项绘制指示符
        {
        case 0:
            OLED_PrintString(0, 0, ">", &font16x16, OLED_COLOR_NORMAL);
            OLED_PrintString(0, 16, " ", &font16x16, OLED_COLOR_NORMAL);
            break;
        case 1:
            OLED_PrintString(0, 0, " ", &font16x16, OLED_COLOR_NORMAL);
            OLED_PrintString(0, 16, ">", &font16x16, OLED_COLOR_NORMAL);
            OLED_PrintString(0, 32, " ", &font16x16, OLED_COLOR_NORMAL);
            break;
        case 2:
            OLED_PrintString(0, 16, " ", &font16x16, OLED_COLOR_NORMAL);
            OLED_PrintString(0, 32, ">", &font16x16, OLED_COLOR_NORMAL);
            OLED_PrintString(0, 48, " ", &font16x16, OLED_COLOR_NORMAL);
            break;
        case 3:
            OLED_PrintString(0, 32, " ", &font16x16, OLED_COLOR_NORMAL);
            OLED_PrintString(0, 48, ">", &font16x16, OLED_COLOR_NORMAL);
            break;
        default:
            break;
        }
        OLED_ShowFrame(); // 显示当前帧
    }
}

/**
 * @brief 显示LED文本并处理按钮输入以切换LED状态
 * @param 无参数
 * @return 0 表示重置
 */
int text_led()
{
    OLED_NewFrame();
    OLED_PrintString(48, 34, "LED", &font16x16, OLED_COLOR_NORMAL);
    OLED_ShowFrame();

    while (1) // 无限循环，等待按钮输入
    {
        if (button_status == BUTTON_RST) // 检测重置按钮
        {
            button_status = 0; // 重置按钮状态
            LED_Off();         // 关闭LED
            return 0;          // 返回0表示重置
        }
        LED_Toggle();    // 切换LED状态
        HAL_Delay(1000); // 延时1秒
    }
}

/**
 * @brief 显示电机速度文本并处理按钮输入以控制电机方向和速度
 * @param 无参数
 * @return 0 表示重置
 */
int text_motor()
{
    extern float motor_speed;  // 外部变量，电机速度
    extern uint8_t READ_SPEED; // 外部变量，是否读取速度
    int motor_flag = 0;        // 初始化电机控制标志

    READ_SPEED = 1;  // 设置读取速度标志
    OLED_NewFrame(); // 创建新的OLED帧
    OLED_PrintString(0, 0, "MOTOR_SPEED", &font16x16, OLED_COLOR_NORMAL); // 绘制“MOTOR_SPEED”文本
    Encoder_Motor_Init(); // 初始化编码器电机

    while (1) // 无限循环，等待按钮输入
    {
        if (motor_flag % 4 == 1) // 检测电机标志以控制电机正转
        {
            motor_positive();                                                      // 电机正转
            OLED_PrintFloat(0, 16, motor_speed, 2, &font16x16, OLED_COLOR_NORMAL); // 绘制电机速度
        }
        else if (motor_flag % 4 == 3) // 检测电机标志以控制电机反转
        {
            motor_reverse();                                                       // 电机反转
            OLED_PrintFloat(0, 32, motor_speed, 2, &font16x16, OLED_COLOR_NORMAL); // 绘制电机速度
        }
        else // 其他情况停止电机
        {
            Encoder_Motor_SetSpeed(3, 0); // 停止电机
        }

        if (button_status == BUTTON_SET) // 检测设置按钮
        {
            button_status = 0; // 重置按钮状态
            motor_flag++;      // 更新电机控制标志
        }
        if (button_status == BUTTON_RST) // 检测重置按钮
        {
            button_status = 0;                       // 重置按钮状态
            HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1); // 停止PWM定时器
            READ_SPEED = 0;                          // 清除读取速度标志
            return 0;                                // 返回0表示重置
        }
        OLED_ShowFrame(); // 显示当前帧
    }
}

/**
 * @brief 显示按钮界面并处理按钮输入以高亮显示当前按下的按钮
 * @param 无参数
 * @return 0 表示重置
 */
int text_button()
{
    uint8_t flag = 0; // 初始化标志，用于检测按钮状态变化

    OLED_NewFrame();
    OLED_PrintString(40, 0, "BUTTON", &font16x16, OLED_COLOR_NORMAL);
    OLED_PrintString(60, 16, "^", &font16x16, OLED_COLOR_NORMAL);
    OLED_PrintString(32, 32, "<", &font16x16, OLED_COLOR_NORMAL);
    OLED_PrintString(60, 32, "O", &font16x16, OLED_COLOR_NORMAL);
    OLED_PrintString(88, 32, ">", &font16x16, OLED_COLOR_NORMAL);
    OLED_PrintString(60, 48, "v", &font16x16, OLED_COLOR_NORMAL);
    OLED_PrintString(0, 48, "SET", &font16x16, OLED_COLOR_NORMAL);
    OLED_PrintString(104, 48, "RST", &font16x16, OLED_COLOR_NORMAL);
    OLED_ShowFrame();

    while (1) // 无限循环，等待按钮输入
    {
        if (button_status == BUTTON_UP) // 检测向上按钮
        {
            button_status = 0;                                              // 重置按钮状态
            OLED_PrintString(60, 16, "^", &font16x16, OLED_COLOR_REVERSED); // 高亮显示向上按钮
            flag = 1; // 设置标志以更新显示
        }
        if (button_status == BUTTON_DOWN) // 检测向下按钮
        {
            button_status = 0;                                              // 重置按钮状态
            OLED_PrintString(60, 48, "v", &font16x16, OLED_COLOR_REVERSED); // 高亮显示向下按钮
            flag = 1; // 设置标志以更新显示
        }
        if (button_status == BUTTON_LEFT) // 检测左按钮
        {
            button_status = 0;                                              // 重置按钮状态
            OLED_PrintString(32, 32, "<", &font16x16, OLED_COLOR_REVERSED); // 高亮显示左按钮
            flag = 1; // 设置标志以更新显示
        }
        if (button_status == BUTTON_RIGHT) // 检测右按钮
        {
            button_status = 0;                                              // 重置按钮状态
            OLED_PrintString(88, 32, ">", &font16x16, OLED_COLOR_REVERSED); // 高亮显示右按钮
            flag = 1; // 设置标志以更新显示
        }
        if (button_status == BUTTON_MID) // 检测中间按钮
        {
            button_status = 0;                                              // 重置按钮状态
            OLED_PrintString(60, 32, "O", &font16x16, OLED_COLOR_REVERSED); // 高亮显示中间按钮
            flag = 1; // 设置标志以更新显示
        }
        if (button_status == BUTTON_SET) // 检测设置按钮
        {
            button_status = 0;                                               // 重置按钮状态
            OLED_PrintString(0, 48, "SET", &font16x16, OLED_COLOR_REVERSED); // 高亮显示“SET”文本
            flag = 1; // 设置标志以更新显示
        }
        if (button_status == BUTTON_RST) // 检测重置按钮
        {
            button_status = 0; // 重置按钮状态
            return 0;          // 返回0表示重置
        }

        if (flag == 1) // 如果有按钮按下
        {
            flag = 0;                                                         // 重置标志
            OLED_ShowFrame();                                                 // 显示当前帧
            HAL_Delay(200);                                                   // 延时200毫秒
            OLED_NewFrame();                                                  // 创建新的OLED帧
            OLED_PrintString(40, 0, "BUTTON", &font16x16, OLED_COLOR_NORMAL); // 绘制“BUTTON”文本
            OLED_PrintString(60, 16, "^", &font16x16, OLED_COLOR_NORMAL); // 绘制向上按钮符号
            OLED_PrintString(32, 32, "<", &font16x16, OLED_COLOR_NORMAL); // 绘制左按钮符号
            OLED_PrintString(60, 32, "O", &font16x16, OLED_COLOR_NORMAL); // 绘制中间按钮符号
            OLED_PrintString(88, 32, ">", &font16x16, OLED_COLOR_NORMAL); // 绘制右按钮符号
            OLED_PrintString(60, 48, "v", &font16x16, OLED_COLOR_NORMAL); // 绘制向下按钮符号
            OLED_PrintString(0, 48, "SET", &font16x16, OLED_COLOR_NORMAL); // 绘制“SET”文本
            OLED_PrintString(104, 48, "RST", &font16x16, OLED_COLOR_NORMAL); // 绘制“RST”文本
            OLED_ShowFrame();                                                // 显示当前帧
        }
    }
}

/**
 * @brief 显示功能选择界面并处理按钮输入以选择不同的功能
 * @param 无参数
 * @return 0 表示重置
 */
int text_function()
{
    int flag       = 0; // 初始化标志，用于选择菜单项
    int menu3_flag = 0; // 初始化标志，用于记录选择的功能

    while (1) // 无限循环，等待按钮输入
    {
        OLED_NewFrame(); // 创建新的OLED帧
        OLED_PrintString(16, 0, "LED        RUN", &font16x16, OLED_COLOR_NORMAL);
        OLED_PrintString(16, 16, "MOTOR      RUN", &font16x16, OLED_COLOR_NORMAL);
        OLED_PrintString(16, 32, "BUTTON     RUN", &font16x16, OLED_COLOR_NORMAL);
        OLED_PrintString(16, 48, "<--", &font16x16, OLED_COLOR_NORMAL);

        if (button_status == BUTTON_DOWN) // 检测向下按钮
        {
            button_status = 0; // 重置按钮状态
            flag++;            // 移动到下一个菜单项
            if (flag > 3)      // 如果超过最大菜单项索引
            {
                flag = 0; // 回到第一个菜单项
            }
        }
        if (button_status == BUTTON_UP) // 检测向上按钮
        {
            button_status = 0; // 重置按钮状态
            flag--;            // 移动到上一个菜单项
            if (flag < 0)      // 如果小于最小菜单项索引
            {
                flag = 3; // 回到最后一个菜单项
            }
        }
        if (button_status == BUTTON_MID) // 检测中间按钮
        {
            button_status = 0;        // 重置按钮状态
            menu3_flag    = flag + 1; // 记录选中的功能
        }
        if (button_status == BUTTON_RST) // 检测重置按钮
        {
            button_status = 0; // 重置按钮状态
            return 0;          // 返回0表示重置
        }

        if (menu3_flag == 1) // 如果选择LED功能
        {
            text_led(); // 调用text_led函数
        }
        if (menu3_flag == 2) // 如果选择MOTOR功能
        {
            text_motor(); // 调用text_motor函数
        }
        if (menu3_flag == 3) // 如果选择BUTTON功能
        {
            text_button(); // 调用text_button函数
        }
        if (menu3_flag == 4) // 如果选择返回
        {
            return 0; // 返回0表示重置
        }
        menu3_flag = 0; // 重置功能选择标志

        switch (flag) // 根据当前选中的菜单项绘制指示符
        {
        case 0:
            OLED_PrintString(0, 0, ">", &font16x16, OLED_COLOR_NORMAL);
            OLED_PrintString(0, 16, " ", &font16x16, OLED_COLOR_NORMAL);
            break;
        case 1:
            OLED_PrintString(0, 0, " ", &font16x16, OLED_COLOR_NORMAL);
            OLED_PrintString(0, 16, ">", &font16x16, OLED_COLOR_NORMAL);
            OLED_PrintString(0, 32, " ", &font16x16, OLED_COLOR_NORMAL);
            break;
        case 2:
            OLED_PrintString(0, 16, " ", &font16x16, OLED_COLOR_NORMAL);
            OLED_PrintString(0, 32, ">", &font16x16, OLED_COLOR_NORMAL);
            OLED_PrintString(0, 48, " ", &font16x16, OLED_COLOR_NORMAL);
            break;
        case 3:
            OLED_PrintString(0, 32, " ", &font16x16, OLED_COLOR_NORMAL);
            OLED_PrintString(0, 48, ">", &font16x16, OLED_COLOR_NORMAL);
            break;
        default:
            break;
        }
        OLED_ShowFrame(); // 显示当前帧
    }
}

/**
 * @brief 显示PID参数KP的调整界面并处理按钮输入以修改KP值
 * @param kp 初始KP值
 * @return 调整后的KP值
 */
float pid_kp(float kp)
{
    float kp_temp = kp; // 初始化kp_temp为传入的kp值
    int selected_digit =
        0; // 0对应小数点后的第二个数字，1对应小数点后的第一个数字，2对应小数点前的第一个数字，3对应整数部分的第一个数字
    char str[12]; // 用于存储kp_temp的字符串表示，预留足够的空间以容纳格式化的浮点数和终止字符

    while (1)
    {
        // 将kp_temp转换为字符串，格式化为五位小数点后两位的浮点数形式
        sprintf(str, "%05.2f", kp_temp);

        // 在OLED屏幕的指定位置显示kp_temp的字符串表示
        OLED_PrintString(40, 0, str, &font16x16, OLED_COLOR_NORMAL);

        // 高亮当前选中的数字
        switch (selected_digit)
        {
        case 0:
            // 在最后一位下画线 并清除相邻位数的下划线
            OLED_DrawLine(40 + 0 * 8, 15, 40 + 1 * 8, 15, OLED_COLOR_REVERSED);
            OLED_DrawLine(40 + 4 * 8, 15, 40 + 5 * 8, 15, OLED_COLOR_NORMAL);
            OLED_DrawLine(40 + 3 * 8, 15, 40 + 4 * 8, 15, OLED_COLOR_REVERSED);
            break;
        case 1:
            // 在倒数第二位下画线 并清除相邻位数的下划线
            OLED_DrawLine(40 + 1 * 8, 15, 40 + 2 * 8, 15, OLED_COLOR_REVERSED);
            OLED_DrawLine(40 + 3 * 8, 15, 40 + 4 * 8, 15, OLED_COLOR_NORMAL);
            OLED_DrawLine(40 + 4 * 8, 15, 40 + 5 * 8, 15, OLED_COLOR_REVERSED);
            break;
        case 2:
            // 在第二位下画线 并清除相邻位数的下划线
            OLED_DrawLine(40 + 0 * 8, 15, 40 + 1 * 8, 15, OLED_COLOR_REVERSED);
            OLED_DrawLine(40 + 1 * 8, 15, 40 + 2 * 8, 15, OLED_COLOR_NORMAL);
            OLED_DrawLine(40 + 3 * 8, 15, 40 + 4 * 8, 15, OLED_COLOR_REVERSED);
            break;
        case 3:
            // 在第一位下画线 并清除相邻位数的下划线
            OLED_DrawLine(40 + 1 * 8, 15, 40 + 2 * 8, 15, OLED_COLOR_REVERSED);
            OLED_DrawLine(40 + 0 * 8, 15, 40 + 1 * 8, 15, OLED_COLOR_NORMAL);
            OLED_DrawLine(40 + 4 * 8, 15, 40 + 5 * 8, 15, OLED_COLOR_REVERSED);
            break;
        }

        // 刷新OLED屏幕以显示更新的界面
        OLED_ShowFrame();

        // 检测按钮状态并更新selected_digit
        if (button_status == BUTTON_LEFT)
        {
            // 选中前一个数字，循环选择
            selected_digit = (selected_digit + 1) % 4;
            // 重置按钮状态
            button_status = 0;
        }
        else if (button_status == BUTTON_RIGHT)
        {
            // 选中后一个数字，循环选择
            selected_digit = (selected_digit - 1 + 4) % 4;
            // 重置按钮状态
            button_status = 0;
        }

        // 根据按钮输入增加selected_digit所指的数字
        if (button_status == BUTTON_UP)
        {
            // 修改选中的数字
            if (selected_digit == 3)
            {
                kp_temp += 10;
            }
            else if (selected_digit == 2)
            {
                kp_temp += 1;
            }
            else if (selected_digit == 1)
            {
                kp_temp += 0.1;
            }
            else if (selected_digit == 0)
            {
                kp_temp += 0.01;
            }
            // 重置按钮状态
            button_status = 0;
        }

        // 根据按钮输入减少selected_digit所指的数字
        if (button_status == BUTTON_DOWN)
        {
            // 修改选中的数字
            if (selected_digit == 3)
            {
                kp_temp -= 10;
            }
            else if (selected_digit == 2)
            {
                kp_temp -= 1;
            }
            else if (selected_digit == 1)
            {
                kp_temp -= 0.1;
            }
            else if (selected_digit == 0)
            {
                kp_temp -= 0.01;
            }
            // 重置按钮状态
            button_status = 0;
        }

        // 确保kp_temp的值在有效范围内[0, 99.99]
        if (kp_temp > 99.99)
        {
            kp_temp = 99.99;
        }
        if (kp_temp < 0)
        {
            kp_temp = 0;
        }

        // 如果重置按钮被按下，返回调整后的KP值
        if (button_status == BUTTON_RST)
        {
            // 重置按钮状态
            button_status = 0;
            return kp_temp;
        }
    }
}

/**
 * @brief 显示PID参数KI的调整界面并处理按钮输入以修改KI值
 * @param ki 初始KI值
 * @return 调整后的KI值
 */
float pid_ki(float ki)
{
    float ki_temp = ki; // 初始化ki_temp为传入的ki值
    int selected_digit =
        0; // 0对应小数点后的第二个数字，1对应小数点后的第一个数字，2对应小数点前的第一个数字，3对应整数部分的第一个数字
    char str[12]; // 用于存储ki_temp的字符串表示，预留足够的空间以容纳格式化的浮点数和终止字符

    while (1)
    {
        // 将ki_temp转换为字符串，格式化为五位小数点后两位的浮点数形式
        sprintf(str, "%05.2f", ki_temp);

        // 在OLED屏幕的指定位置显示ki_temp的字符串表示
        OLED_PrintString(40, 16, str, &font16x16, OLED_COLOR_NORMAL);

        // 高亮当前选中的数字
        switch (selected_digit)
        {
        case 0:
            // 在最后一位下画线 并清除相邻位数的下划线
            OLED_DrawLine(40 + 0 * 8, 31, 40 + 1 * 8, 31, OLED_COLOR_REVERSED);
            OLED_DrawLine(40 + 4 * 8, 31, 40 + 5 * 8, 31, OLED_COLOR_NORMAL);
            OLED_DrawLine(40 + 3 * 8, 31, 40 + 4 * 8, 31, OLED_COLOR_REVERSED);
            break;
        case 1:
            // 在倒数第二位下画线 并清除相邻位数的下划线
            OLED_DrawLine(40 + 1 * 8, 31, 40 + 2 * 8, 31, OLED_COLOR_REVERSED);
            OLED_DrawLine(40 + 3 * 8, 31, 40 + 4 * 8, 31, OLED_COLOR_NORMAL);
            OLED_DrawLine(40 + 4 * 8, 31, 40 + 5 * 8, 31, OLED_COLOR_REVERSED);
            break;
        case 2:
            // 在第二位下画线 并清除相邻位数的下划线
            OLED_DrawLine(40 + 0 * 8, 31, 40 + 1 * 8, 31, OLED_COLOR_REVERSED);
            OLED_DrawLine(40 + 1 * 8, 31, 40 + 2 * 8, 31, OLED_COLOR_NORMAL);
            OLED_DrawLine(40 + 3 * 8, 31, 40 + 4 * 8, 31, OLED_COLOR_REVERSED);
            break;
        case 3:
            // 在第一位下画线 并清除相邻位数的下划线
            OLED_DrawLine(40 + 1 * 8, 31, 40 + 2 * 8, 31, OLED_COLOR_REVERSED);
            OLED_DrawLine(40 + 0 * 8, 31, 40 + 1 * 8, 31, OLED_COLOR_NORMAL);
            OLED_DrawLine(40 + 4 * 8, 31, 40 + 5 * 8, 31, OLED_COLOR_REVERSED);
            break;
        }

        // 刷新OLED屏幕以显示更新的界面
        OLED_ShowFrame();

        // 检测按钮状态并更新selected_digit
        if (button_status == BUTTON_LEFT)
        {
            // 选中前一个数字，循环选择
            selected_digit = (selected_digit + 1) % 4;
            // 重置按钮状态
            button_status = 0;
        }
        else if (button_status == BUTTON_RIGHT)
        {
            // 选中后一个数字，循环选择
            selected_digit = (selected_digit - 1 + 4) % 4;
            // 重置按钮状态
            button_status = 0;
        }

        // 根据按钮输入增加selected_digit所指的数字
        if (button_status == BUTTON_UP)
        {
            // 修改选中的数字
            if (selected_digit == 3)
            {
                ki_temp += 10;
            }
            else if (selected_digit == 2)
            {
                ki_temp += 1;
            }
            else if (selected_digit == 1)
            {
                ki_temp += 0.1;
            }
            else if (selected_digit == 0)
            {
                ki_temp += 0.01;
            }
            // 重置按钮状态
            button_status = 0;
        }

        // 根据按钮输入减少selected_digit所指的数字
        if (button_status == BUTTON_DOWN)
        {
            // 修改选中的数字
            if (selected_digit == 3)
            {
                ki_temp -= 10;
            }
            else if (selected_digit == 2)
            {
                ki_temp -= 1;
            }
            else if (selected_digit == 1)
            {
                ki_temp -= 0.1;
            }
            else if (selected_digit == 0)
            {
                ki_temp -= 0.01;
            }
            // 重置按钮状态
            button_status = 0;
        }

        // 确保ki_temp的值在有效范围内[0, 99.99]
        if (ki_temp > 99.99)
        {
            ki_temp = 99.99;
        }
        if (ki_temp < 0)
        {
            ki_temp = 0;
        }

        // 如果重置按钮被按下，返回调整后的KI值
        if (button_status == BUTTON_RST)
        {
            // 重置按钮状态
            button_status = 0;
            return ki_temp;
        }
    }
}

/**
 * @brief 显示PID参数KD的调整界面并处理按钮输入以修改KD值
 * @param kd 初始KD值
 * @return 调整后的KD值
 */
float pid_kd(float kd)
{
    float kd_temp = kd; // 初始化kd_temp为传入的kd值
    int selected_digit =
        0; // 0对应小数点后的第二个数字，1对应小数点后的第一个数字，2对应小数点前的第一个数字，3对应整数部分的第一个数字
    char str[12]; // 用于存储kd_temp的字符串表示，预留足够的空间以容纳格式化的浮点数和终止字符

    while (1)
    {
        // 将kd_temp转换为字符串，格式化为五位小数点后两位的浮点数形式
        sprintf(str, "%05.2f", kd_temp);

        // 在OLED屏幕的指定位置显示kd_temp的字符串表示
        OLED_PrintString(40, 32, str, &font16x16, OLED_COLOR_NORMAL);

        // 高亮当前选中的数字  实现方式同pid_kp
        switch (selected_digit)
        {
        case 0:
            OLED_DrawLine(40 + 0 * 8, 47, 40 + 1 * 8, 47, OLED_COLOR_REVERSED);
            OLED_DrawLine(40 + 4 * 8, 47, 40 + 5 * 8, 47, OLED_COLOR_NORMAL);
            OLED_DrawLine(40 + 3 * 8, 47, 40 + 4 * 8, 47, OLED_COLOR_REVERSED);
            break;
        case 1:
            OLED_DrawLine(40 + 1 * 8, 47, 40 + 2 * 8, 47, OLED_COLOR_REVERSED);
            OLED_DrawLine(40 + 3 * 8, 47, 40 + 4 * 8, 47, OLED_COLOR_NORMAL);
            OLED_DrawLine(40 + 4 * 8, 47, 40 + 5 * 8, 47, OLED_COLOR_REVERSED);
            break;
        case 2:
            OLED_DrawLine(40 + 0 * 8, 47, 40 + 1 * 8, 47, OLED_COLOR_REVERSED);
            OLED_DrawLine(40 + 1 * 8, 47, 40 + 2 * 8, 47, OLED_COLOR_NORMAL);
            OLED_DrawLine(40 + 3 * 8, 47, 40 + 4 * 8, 47, OLED_COLOR_REVERSED);
            break;
        case 3:
            OLED_DrawLine(40 + 1 * 8, 47, 40 + 2 * 8, 47, OLED_COLOR_REVERSED);
            OLED_DrawLine(40 + 0 * 8, 47, 40 + 1 * 8, 47, OLED_COLOR_NORMAL);
            OLED_DrawLine(40 + 4 * 8, 47, 40 + 5 * 8, 47, OLED_COLOR_REVERSED);
            break;
        }

        // 刷新OLED屏幕以显示更新的界面
        OLED_ShowFrame();

        // 检测按钮状态并更新selected_digit
        if (button_status == BUTTON_LEFT)
        {
            // 选中前一个数字，循环选择
            selected_digit = (selected_digit + 1) % 4;
            // 重置按钮状态
            button_status = 0;
        }
        else if (button_status == BUTTON_RIGHT)
        {
            // 选中后一个数字，循环选择
            selected_digit = (selected_digit - 1 + 4) % 4;
            // 重置按钮状态
            button_status = 0;
        }

        // 根据按钮输入增加selected_digit所指的数字
        if (button_status == BUTTON_UP)
        {
            // 修改选中的数字
            if (selected_digit == 3)
            {
                kd_temp += 10;
            }
            else if (selected_digit == 2)
            {
                kd_temp += 1;
            }
            else if (selected_digit == 1)
            {
                kd_temp += 0.1;
            }
            else if (selected_digit == 0)
            {
                kd_temp += 0.01;
            }
            // 重置按钮状态
            button_status = 0;
        }

        // 根据按钮输入减少selected_digit所指的数字
        if (button_status == BUTTON_DOWN)
        {
            // 修改选中的数字
            if (selected_digit == 3)
            {
                kd_temp -= 10;
            }
            else if (selected_digit == 2)
            {
                kd_temp -= 1;
            }
            else if (selected_digit == 1)
            {
                kd_temp -= 0.1;
            }
            else if (selected_digit == 0)
            {
                kd_temp -= 0.01;
            }
            // 重置按钮状态
            button_status = 0;
        }

        // 确保kd_temp的值在有效范围内[0, 99.99]
        if (kd_temp > 99.99)
        {
            kd_temp = 99.99;
        }
        if (kd_temp < 0)
        {
            kd_temp = 0;
        }

        // 如果重置按钮被按下，返回调整后的KD值
        if (button_status == BUTTON_RST)
        {
            // 重置按钮状态
            button_status = 0;
            return kd_temp;
        }
    }
}

/**
 * @brief 显示PID参数TS的调整界面并处理按钮输入以修改TS值
 * @param ts 初始TS值
 * @return 调整后的TS值
 */
float pid_ts(float ts)
{
    float ts_temp = ts; // 初始化ts_temp为传入的ts值
    int selected_digit =
        0; // 0对应小数点后的第一个数字，1对应小数点前的第一个数字，2对应小数点前的第二个数字，3对应整数部分的第一个数字
    char str[12]; // 用于存储ts_temp的字符串表示，预留足够的空间以容纳格式化的浮点数和终止字符

    while (1)
    {
        // 将ts_temp转换为字符串，格式化为五位小数点后一位的浮点数形式
        if (ts_temp >= 0)
        {
            sprintf(str, "+%04d", (int)ts_temp);
        }
        else
        {
            sprintf(str, "%05d", (int)ts_temp);
        }

        // 在OLED屏幕的指定位置显示ts_temp的字符串表示

        OLED_PrintString(40, 48, str, &font16x16, OLED_COLOR_NORMAL);

        switch (selected_digit)
        {
        case 0:
            OLED_DrawLine(40 + 1 * 8, 63, 40 + 2 * 8, 63, OLED_COLOR_REVERSED);
            OLED_DrawLine(40 + 4 * 8, 63, 40 + 5 * 8, 63, OLED_COLOR_NORMAL);
            OLED_DrawLine(40 + 3 * 8, 63, 40 + 4 * 8, 63, OLED_COLOR_REVERSED);
            break;
        case 1:
            OLED_DrawLine(40 + 2 * 8, 63, 40 + 3 * 8, 63, OLED_COLOR_REVERSED);
            OLED_DrawLine(40 + 3 * 8, 63, 40 + 4 * 8, 63, OLED_COLOR_NORMAL);
            OLED_DrawLine(40 + 4 * 8, 63, 40 + 5 * 8, 63, OLED_COLOR_REVERSED);
            break;
        case 2:
            OLED_DrawLine(40 + 1 * 8, 63, 40 + 2 * 8, 63, OLED_COLOR_REVERSED);
            OLED_DrawLine(40 + 2 * 8, 63, 40 + 3 * 8, 63, OLED_COLOR_NORMAL);
            OLED_DrawLine(40 + 3 * 8, 63, 40 + 4 * 8, 63, OLED_COLOR_REVERSED);
            break;
        case 3:
            OLED_DrawLine(40 + 4 * 8, 63, 40 + 5 * 8, 63, OLED_COLOR_REVERSED);
            OLED_DrawLine(40 + 1 * 8, 63, 40 + 2 * 8, 63, OLED_COLOR_NORMAL);
            OLED_DrawLine(40 + 2 * 8, 63, 40 + 3 * 8, 63, OLED_COLOR_REVERSED);
            break;

            break;
        }
        // 刷新OLED屏幕以显示更新的界面
        OLED_ShowFrame();

        // 检测按钮状态并更新selected_digit
        if (button_status == BUTTON_LEFT)
        {
            // 选中前一个数字，循环选择
            selected_digit = (selected_digit + 1) % 4;
            // 重置按钮状态
            button_status = 0;
        }
        else if (button_status == BUTTON_RIGHT)
        {
            // 选中后一个数字，循环选择
            selected_digit = (selected_digit - 1 + 4) % 4;
            // 重置按钮状态
            button_status = 0;
        }

        // 根据按钮输入增加selected_digit所指的数字
        if (button_status == BUTTON_UP)
        {
            // 修改选中的数字
            if (selected_digit == 3)
            {
                ts_temp += 1000;
            }
            else if (selected_digit == 2)
            {
                ts_temp += 100;
            }
            else if (selected_digit == 1)
            {
                ts_temp += 10;
            }
            else if (selected_digit == 0)
            {
                ts_temp += 1;
            }
            // 重置按钮状态
            button_status = 0;
        }

        // 根据按钮输入减少selected_digit所指的数字
        if (button_status == BUTTON_DOWN)
        {
            // 修改选中的数字
            if (selected_digit == 3)
            {
                ts_temp -= 1000;
            }
            else if (selected_digit == 2)
            {
                ts_temp -= 100;
            }
            else if (selected_digit == 1)
            {
                ts_temp -= 10;
            }
            else if (selected_digit == 0)
            {
                ts_temp -= 1;
            }
            // 重置按钮状态
            button_status = 0;
        }

        // 确保ts_temp的值在有效范围内[0, 999.9]
        if (ts_temp > 9999.0)
        {
            ts_temp = 9999.0;
        }
        if (ts_temp < -9999.0)
        {
            ts_temp = -9999.0;
        }

        // 如果重置按钮被按下，返回调整后的TS值
        if (button_status == BUTTON_RST)
        {
            // 重置按钮状态
            button_status = 0;
            return ts_temp;
        }
    }
}

/**
 * @brief 运行PID控制器并处理按钮输入以控制电机速度
 * @param kp 比例系数
 * @param ki 积分系数
 * @param kd 微分系数
 * @param ts 目标速度
 * @param T 采样时间
 * @param out_min 输出最小值
 * @param out_max 输出最大值
 * @param tau 滤波时间常数
 */
void pid_run(float kp, float ki, float kd, float ts, float T, float out_min, float out_max,
             float tau)
{
    int show_x = 0, ts_y = 0, vs_y = 0, show_flag = 0;
    extern PIDController pid;  // 声明外部PID控制器结构体
    extern float target_speed; // 声明外部目标速度变量
    extern float motor_speed;  // 声明外部电机速度变量
    extern uint8_t READ_SPEED; // 声明外部读取速度标志变量

    target_speed = ts;    // 设置目标速度为传入的ts值
    READ_SPEED   = 0;     // 重置读取速度标志
    Encoder_Motor_Init(); // 初始化编码器和电机

    // 初始化PID控制器参数
    pid_init(&pid, kp, ki, kd, T, out_min, out_max, tau);

    // 清除OLED屏幕并绘制初始线条
    OLED_NewFrame();
    OLED_DrawLine(0, 0, 0, 50, OLED_COLOR_NORMAL);    // 绘制左侧垂直线
    OLED_DrawLine(0, 50, 120, 50, OLED_COLOR_NORMAL); // 绘制底部水平线

    char str_ts[12]; // 用于存储目标速度的字符串表示
    char str_vs[12]; // 用于存储电机速度的字符串表示

    while (1)
    {
        // 将目标速度和电机速度转换为字符串格式
        sprintf(str_ts, "%.2f", target_speed);
        sprintf(str_vs, "%.2f", motor_speed);

        // 如果重置按钮被按下
        if (button_status == BUTTON_RST)
        {
            HAL_TIM_Base_Stop_IT(&htim2);            // 停止TIM2的中断
            pid_reset(&pid);                         // 重置PID控制器
            HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1); // 停止TIM1的PWM模式通道1

            button_status = 0;   // 重置按钮状态
            READ_SPEED    = 0;   // 重置读取速度标志
            motor_speed   = 0.0; // 重置电机速度
            return;              // 退出函数
        }

        // 如果设置按钮被按下
        if (button_status == BUTTON_SET)
        {
            button_status = 0; // 重置按钮状态
            show_flag     = 1; // 设置显示标志
        }

        // 打印目标速度和电机速度到串口
        printf("%.2f,%.2f\r\n", target_speed, motor_speed);

        // 计算目标速度和电机速度在OLED屏幕上的显示位置
        if (target_speed >= 0)
        {
            ts_y = (target_speed / out_max) * 50;
            vs_y = (motor_speed / out_max) * 50;
        }
        else
        {
            ts_y = (target_speed / out_min) * 50;
            vs_y = (motor_speed / out_min) * 50;
        }

        // 如果显示标志为0，则在OLED屏幕上绘制速度曲线
        if (show_flag == 0)
        {
            OLED_SetPixel(show_x, 50 - ts_y, OLED_COLOR_NORMAL); // 绘制目标速度曲线
            OLED_SetPixel(show_x, 50 - vs_y, OLED_COLOR_NORMAL); // 绘制电机速度曲线
            show_x++;                                            // 移动到下一个显示位置

            // 如果显示位置超过屏幕宽度，重置屏幕并重新绘制初始线条
            if (show_x > 120)
            {
                OLED_NewFrame();
                OLED_DrawLine(0, 0, 0, 50, OLED_COLOR_NORMAL);    // 绘制左侧垂直线
                OLED_DrawLine(0, 50, 120, 50, OLED_COLOR_NORMAL); // 绘制底部水平线
                show_x = 0;                                       // 重置显示位置
            }
        }

        // 在OLED屏幕上显示目标速度和电机速度的标签和值
        OLED_PrintASCIIString(0, 56, "TS:", &afont8x6, OLED_COLOR_NORMAL); // 显示目标速度标签
        OLED_PrintASCIIString(64, 56, "VS:", &afont8x6, OLED_COLOR_NORMAL); // 显示电机速度标签
        OLED_PrintASCIIString(20, 56, str_ts, &afont8x6, OLED_COLOR_NORMAL); // 显示目标速度值
        OLED_PrintASCIIString(84, 56, str_vs, &afont8x6, OLED_COLOR_NORMAL); // 显示电机速度值

        // 刷新OLED屏幕以显示更新的界面
        OLED_ShowFrame();
    }
}

/**
 * @brief 显示PID参数调整界面并处理按钮输入以选择和调整PID参数及运行PID控制器
 * @param 无参数
 * @return 0 表示重置
 */
int pid_function()
{
    int flag = 0, menu3_flag = 0;
    float kp = 1.0, ki = 5.0, kd = 0.0, ts = 0.0;
    char str_kp[12]; // 用于存储kp的字符串表示
    char str_ki[12]; // 用于存储ki的字符串表示
    char str_kd[12]; // 用于存储kd的字符串表示
    char str_ts[12]; // 用于存储ts的字符串表示

    while (1)
    {
        // 将kp、ki、kd和ts转换为字符串
        sprintf(str_kp, "%05.2f", kp);
        sprintf(str_ki, "%05.2f", ki);
        sprintf(str_kd, "%05.2f", kd);
        if (ts >= 0)
        {
            sprintf(str_ts, "+%04d", (int)ts);
        }
        else
        {
            sprintf(str_ts, "%05d", (int)ts);
        }

        // 清除OLED屏幕并绘制初始界面
        OLED_NewFrame();
        OLED_PrintString(16, 0, "KP", &font16x16, OLED_COLOR_NORMAL);
        OLED_PrintString(40, 0, str_kp, &font16x16, OLED_COLOR_NORMAL);
        OLED_PrintString(100, 0, "RUN", &font16x16, OLED_COLOR_NORMAL);

        OLED_PrintString(16, 16, "KI", &font16x16, OLED_COLOR_NORMAL);
        OLED_PrintString(40, 16, str_ki, &font16x16, OLED_COLOR_NORMAL);

        OLED_PrintString(16, 32, "KD", &font16x16, OLED_COLOR_NORMAL);
        OLED_PrintString(40, 32, str_kd, &font16x16, OLED_COLOR_NORMAL);

        OLED_PrintString(16, 48, "TS", &font16x16, OLED_COLOR_NORMAL);

        OLED_PrintString(40, 48, str_ts, &font16x16, OLED_COLOR_NORMAL);

        // 检测按钮状态并更新flag
        if (button_status == BUTTON_DOWN)
        {
            button_status = 0; // 重置按钮状态
            flag++;            // 选择下一个菜单项
            if (flag > 4)
            {
                flag = 0; // 循环选择
            }
        }
        else if (button_status == BUTTON_UP)
        {
            button_status = 0; // 重置按钮状态
            flag--;            // 选择上一个菜单项
            if (flag < 0)
            {
                flag = 4; // 循环选择
            }
        }

        // 如果中间按钮被按下，设置menu3_flag以进入相应的调整界面
        if (button_status == BUTTON_MID)
        {
            button_status = 0;        // 重置按钮状态
            menu3_flag    = flag + 1; // 设置menu3_flag
        }

        // 如果重置按钮被按下，返回0以重置
        if (button_status == BUTTON_RST)
        {
            button_status = 0; // 重置按钮状态
            return 0;          // 退出函数
        }
        // 如果设置按钮被按下，直接进入运行界面
        if (button_status == BUTTON_SET)
        {
            button_status = 0; // 重置按钮状态
            menu3_flag    = 5; // 设置menu3_flag为5
        }

        // 根据menu3_flag进入相应的调整界面
        if (menu3_flag == 1)
        {
            kp = pid_kp(kp); // 调整kp值
        }
        else if (menu3_flag == 2)
        {
            ki = pid_ki(ki); // 调整ki值
        }
        else if (menu3_flag == 3)
        {
            kd = pid_kd(kd); // 调整kd值
        }
        else if (menu3_flag == 4)
        {
            ts = pid_ts(ts); // 调整ts值
        }
        else if (menu3_flag == 5)
        {
            // 运行PID控制器
            pid_run(kp, ki, kd, ts, 0.1, -FULL_SPEED_RPM, FULL_SPEED_RPM, 0.3);
            Encoder_Motor_SetSpeed(3, 0);
        }

        // 重置menu3_flag
        menu3_flag = 0;

        // 根据flag高亮当前选中的菜单项
        switch (flag)
        {
        case 0:
            OLED_PrintString(0, 0, ">", &font16x16, OLED_COLOR_NORMAL);  // 高亮KP
            OLED_PrintString(0, 16, " ", &font16x16, OLED_COLOR_NORMAL); // 取消高亮KI
            OLED_PrintString(0, 32, " ", &font16x16, OLED_COLOR_NORMAL); // 取消高亮KD
            OLED_PrintString(0, 48, " ", &font16x16, OLED_COLOR_NORMAL); // 取消高亮TS
            break;
        case 1:
            OLED_PrintString(0, 0, " ", &font16x16, OLED_COLOR_NORMAL);  // 取消高亮KP
            OLED_PrintString(0, 16, ">", &font16x16, OLED_COLOR_NORMAL); // 高亮KI
            OLED_PrintString(0, 32, " ", &font16x16, OLED_COLOR_NORMAL); // 取消高亮KD
            OLED_PrintString(0, 48, " ", &font16x16, OLED_COLOR_NORMAL); // 取消高亮TS
            break;
        case 2:
            OLED_PrintString(0, 0, " ", &font16x16, OLED_COLOR_NORMAL);  // 取消高亮KP
            OLED_PrintString(0, 16, " ", &font16x16, OLED_COLOR_NORMAL); // 取消高亮KI
            OLED_PrintString(0, 32, ">", &font16x16, OLED_COLOR_NORMAL); // 高亮KD
            OLED_PrintString(0, 48, " ", &font16x16, OLED_COLOR_NORMAL); // 取消高亮TS
            break;
        case 3:
            OLED_PrintString(0, 0, " ", &font16x16, OLED_COLOR_NORMAL);  // 取消高亮KP
            OLED_PrintString(0, 16, " ", &font16x16, OLED_COLOR_NORMAL); // 取消高亮KI
            OLED_PrintString(0, 32, " ", &font16x16, OLED_COLOR_NORMAL); // 取消高亮KD
            OLED_PrintString(0, 48, ">", &font16x16, OLED_COLOR_NORMAL); // 高亮TS
            break;
        case 4:
            OLED_PrintString(90, 0, ">", &font16x16, OLED_COLOR_NORMAL); // 高亮RUN
            OLED_PrintString(0, 0, " ", &font16x16, OLED_COLOR_NORMAL);  // 取消高亮KP
            OLED_PrintString(0, 16, " ", &font16x16, OLED_COLOR_NORMAL); // 取消高亮KI
            OLED_PrintString(0, 32, " ", &font16x16, OLED_COLOR_NORMAL); // 取消高亮KD
            OLED_PrintString(0, 48, " ", &font16x16, OLED_COLOR_NORMAL); // 取消高亮TS
            break;
        default:
            break;
        }

        // 刷新OLED屏幕以显示更新的界面
        OLED_ShowFrame();
    }
    return 0;
}

/**
 * @brief 在OLED显示屏上显示MPU6050传感器数据，并处理按钮输入以重置函数。
 * @param 无参数
 * @return 如果重置按钮按下则返回0，表示重置；否则持续显示传感器数据。
 */
int mpu_function()
{
    // 初始化OLED显示帧
    OLED_NewFrame();
    // 在OLED显示屏上打印MPU6050标签
    OLED_PrintString(0, 0, "MPU6050", &font16x16, OLED_COLOR_NORMAL);
    // 在OLED显示屏上打印加速度计X轴标签
    OLED_PrintString(0, 16, "AX:", &font16x16, OLED_COLOR_NORMAL);
    // 在OLED显示屏上打印加速度计Y轴标签
    OLED_PrintString(0, 32, "AY:", &font16x16, OLED_COLOR_NORMAL);
    // 在OLED显示屏上打印加速度计Z轴标签
    OLED_PrintString(0, 48, "AZ:", &font16x16, OLED_COLOR_NORMAL);
    // 在OLED显示屏上打印陀螺仪X轴标签
    OLED_PrintString(64, 16, "GX:", &font16x16, OLED_COLOR_NORMAL);
    // 在OLED显示屏上打印陀螺仪Y轴标签
    OLED_PrintString(64, 32, "GY:", &font16x16, OLED_COLOR_NORMAL);
    // 在OLED显示屏上打印陀螺仪Z轴标签
    OLED_PrintString(64, 48, "GZ:", &font16x16, OLED_COLOR_NORMAL);

    // 声明外部MPU6050数据结构
    extern MPU6050_Data mpu6050Data;
    // 声明外部MPU读取标志
    extern uint8_t MPU_READ;
    // 定义HAL状态变量
    HAL_StatusTypeDef temp;
    // 定义MPU初始化完成标志
    int MPU_READY = 0;

    // 初始化MPU6050传感器
    temp = MPU6050_Init();
    // 检查MPU6050初始化是否成功
    if (temp == HAL_OK)
    {
        // 初始化成功，在OLED显示屏上打印"OK"
        OLED_PrintString(104, 0, "OK", &font16x16, OLED_COLOR_NORMAL);
        MPU_READY = 1;
    }
    else
    {
        // 初始化失败，在OLED显示屏上打印"NO"
        OLED_PrintString(104, 0, "NO", &font16x16, OLED_COLOR_NORMAL);
        MPU_READY = 2; // 设置MPU_READY为2，表示初始化失败
    }

    // 设置MPU读取标志为真
    MPU_READ = MPU_READY == 1 ? 1 : 0;

    // 主循环，持续读取并显示MPU6050传感器数据
    while (1)
    {
        // 检查按钮状态是否为复位状态
        if (button_status == BUTTON_RST)
        {
            // 按钮按下，重置按钮状态和MPU读取标志，退出函数
            button_status = 0;
            MPU_READ      = 0;
            return 0;
        }

        // 如果MPU已经初始化成功
        if (MPU_READY == 1)
        {
            // 在OLED显示屏上打印加速度计数据
            OLED_PrintFloat(24, 16, mpu6050Data.Ax, 2, &font16x16, OLED_COLOR_NORMAL);
            OLED_PrintFloat(24, 32, mpu6050Data.Ay, 2, &font16x16, OLED_COLOR_NORMAL);
            OLED_PrintFloat(24, 48, mpu6050Data.Az, 2, &font16x16, OLED_COLOR_NORMAL);
            // 在OLED显示屏上打印陀螺仪数据
            OLED_PrintFloat(88, 16, mpu6050Data.Gx, 2, &font16x16, OLED_COLOR_NORMAL);
            OLED_PrintFloat(88, 32, mpu6050Data.Gy, 2, &font16x16, OLED_COLOR_NORMAL);
            OLED_PrintFloat(88, 48, mpu6050Data.Gz, 2, &font16x16, OLED_COLOR_NORMAL);
            // 通过串口打印加速度计和陀螺仪数据
            printf("%.2f,%.2f,%.2f,%.2f,%.2f,%.2f\r\n", mpu6050Data.Ax, mpu6050Data.Ay,
                   mpu6050Data.Az, mpu6050Data.Gx, mpu6050Data.Gy, mpu6050Data.Gz);
        }
        else if (MPU_READY == 2)
        {
            // 如果MPU初始化失败，通过串口打印错误信息
            printf("MPU INIT ERROR\r\n");
        }

        // 更新OLED显示屏内容
        OLED_ShowFrame();
        // 延时20毫秒
        HAL_Delay(20);
    }

    // 返回0，表示函数正常结束
    return 0;
}
