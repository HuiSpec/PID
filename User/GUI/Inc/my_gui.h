// button.h
#ifndef _MYGUI_H_
#define _MYGUI_H_


#include "button.h"
#include "main.h"
#include "oled_driver.h"
#include "led.h"
#include "encoder.h"
#include <stdbool.h>
#include "mpu6050.h"
#include "usart.h"

int logo();
int main_menu(int up_menu);
int text_function();
int pid_function();
int mpu_function();

#endif 
