#ifndef __MPU6050_H
#define __MPU6050_H

#include "main.h"
#include <math.h>
#include <stdio.h>

typedef struct
{
    float Ax;
    float Ay;
    float Az;
    float Gx;
    float Gy;
    float Gz;
    float Temperature;
} MPU6050_Data;

/* MPU6050 寄存器地址宏定义 */
#define MPU6050_ADDR (0x68 << 1) // AD0接地时的I2C地址(8位)
#define WHO_AM_I_REG 0x75
#define PWR_MGMT_1_REG 0x6B
#define SMPLRT_DIV_REG 0x19
#define CONFIG_REG 0x1A
#define GYRO_CONFIG_REG 0x1B
#define ACCEL_CONFIG_REG 0x1C

extern MPU6050_Data mpu6050Data;

HAL_StatusTypeDef MPU6050_Init(void);

void MPU6050_Read_All(MPU6050_Data* data);

#endif
