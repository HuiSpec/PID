// button.c
#include "button.h"

// 定义按钮句柄
Button button1_handle, button2_handle, button3_handle, button4_handle, button5_handle,
    button6_handle, button7_handle;

uint8_t button_status = 0x00;

// 定义HAL函数读取GPIO电平
uint8_t hal_button_level(uint8_t button_id)
{
    switch (button_id)
    {
    case 1:
        return HAL_GPIO_ReadPin(BUTTON_REST_GPIO_Port, BUTTON_REST_Pin);
    case 2:
        return HAL_GPIO_ReadPin(BUTTON_SET_GPIO_Port, BUTTON_SET_Pin);
    case 3:
        return HAL_GPIO_ReadPin(BUTTON_M_GPIO_Port, BUTTON_M_Pin);
    case 4:
        return HAL_GPIO_ReadPin(BUTTON_R_GPIO_Port, BUTTON_R_Pin);
    case 5:
        return HAL_GPIO_ReadPin(BUTTON_L_GPIO_Port, BUTTON_L_Pin);
    case 6:
        return HAL_GPIO_ReadPin(BUTTON_D_GPIO_Port, BUTTON_D_Pin);
    case 7:
        return HAL_GPIO_ReadPin(BUTTON_U_GPIO_Port, BUTTON_U_Pin);
    default:
        return 0;
    }
}

// 单击回调函数实现
void button1_single_click_callback(Button* btn_handle)
{
    // 按键1单击回调函数实现
    // 示例：HAL_GPIO_TogglePin(GPIO_PORT, GPIO_PIN);
    // printf("***> key1 single_click! <***\r\n");

    button_status = BUTTON_RST;
}

void button2_single_click_callback(Button* btn_handle)
{
    // 按键2单击回调函数实现
    // printf("***> key2 single_click! <***\r\n");
    button_status = BUTTON_SET;
}

void button3_single_click_callback(Button* btn_handle)
{
    // 按键3单击回调函数实现
    // printf("***> key3 single_click! <***\r\n");
    button_status = BUTTON_MID;
}

void button4_single_click_callback(Button* btn_handle)
{
    // 按键4单击回调函数实现
    // printf("***> key4 single_click! <***\r\n");
    button_status =BUTTON_RIGHT;
}

void button5_single_click_callback(Button* btn_handle)
{
    // 按键5单击回调函数实现
    // printf("***> key5 single_click! <***\r\n");
    button_status = BUTTON_LEFT;
}
void button6_single_click_callback(Button* btn_handle)
{
    // 按键6单击回调函数实现
    // printf("***> key6 single_click! <***\r\n");
    button_status = BUTTON_DOWN;
}

void button7_single_click_callback(Button* btn_handle)
{
    // 按键7单击回调函数实现
    // printf("***> key7 single_click! <***\r\n");
    button_status = BUTTON_UP;
}

// 初始化按钮函数
void init_buttons(void)
{
    // 初始化按键1
    button_init(&button1_handle, hal_button_level, 0,
                1); // 假设按键按下时电平为1
    button_attach(&button1_handle, BTN_SINGLE_CLICK, button1_single_click_callback);

    // 初始化按键2
    button_init(&button2_handle, hal_button_level, 0, 2);
    button_attach(&button2_handle, BTN_SINGLE_CLICK, button2_single_click_callback);

    // 初始化按键3
    button_init(&button3_handle, hal_button_level, 0, 3);
    button_attach(&button3_handle, BTN_SINGLE_CLICK, button3_single_click_callback);

    // 初始化按键4
    button_init(&button4_handle, hal_button_level, 0, 4);
    button_attach(&button4_handle, BTN_SINGLE_CLICK, button4_single_click_callback);

    // 初始化按键5
    button_init(&button5_handle, hal_button_level, 0, 5);
    button_attach(&button5_handle, BTN_SINGLE_CLICK, button5_single_click_callback);

    // 初始化按键6
    button_init(&button6_handle, hal_button_level, 0, 6);
    button_attach(&button6_handle, BTN_SINGLE_CLICK, button6_single_click_callback);

    // 初始化按键7
    button_init(&button7_handle, hal_button_level, 0, 7);
    button_attach(&button7_handle, BTN_SINGLE_CLICK, button7_single_click_callback);

    // 启动所有按钮
    button_start(&button1_handle);
    button_start(&button2_handle);
    button_start(&button3_handle);
    button_start(&button4_handle);
    button_start(&button5_handle);
    button_start(&button6_handle);
    button_start(&button7_handle);
}

// // 在主循环中调用button_ticks函数
// void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
//     if (htim->Instance == TIM2) { // 假设使用TIM2定时器，间隔为5ms
//         button_ticks();
//     }
// }
