/**
 * @file    mpu6050.c
 * @brief   MPU6050 传感器驱动程序实现文件
 * @author  HuiSpec
 * @date    2025-09-01
 * @version 1.0.0
 * 
 * @details 该文件包含了 MPU6050 传感器的初始化和数据读取功能的实现。
 *          MPU6050 是一款结合了 3 轴加速度计、3 轴陀螺仪和温度传感器的 MEMS 设备。
 *          该驱动程序利用 STM32 HAL 库实现了与 MPU6050 的通信。
 * 
 * @note    使用前请确保已正确配置 I2C 接口。
 * 
 * @copyright Copyright © 2025 HuiSpec. All rights reserved.
 */



#include "mpu6050.h"
#include "i2c.h"
MPU6050_Data mpu6050Data;
/* 原始数据全局变量 */
int16_t Accel_X_RAW, Accel_Y_RAW, Accel_Z_RAW;
int16_t Gyro_X_RAW, Gyro_Y_RAW, Gyro_Z_RAW;
int16_t Temp_RAW;

/* 转换后的物理量全局变量 */
float Ax, Ay, Az;
float Gx, Gy, Gz;
float Temperature;

/* 初始化 MPU6050 传感器 */
HAL_StatusTypeDef MPU6050_Init(void)
{
    uint8_t check, data;
    // 检查 WHO_AM_I
    if (HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, WHO_AM_I_REG, 1, &check, 1, 100) != HAL_OK)
    {
        return HAL_ERROR;
    }
    if (check != 0x68)
    {
        return HAL_ERROR; // 设备ID不匹配
    }
    // 唤醒传感器
    data = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, PWR_MGMT_1_REG, 1, &data, 1, 100);
    HAL_Delay(10);
    // 设置采样率 1kHz
    data = 0x07;
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, SMPLRT_DIV_REG, 1, &data, 1, 100);
    // 配置 DLPF (44Hz)
    data = 0x03;
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, CONFIG_REG, 1, &data, 1, 100);
    // 陀螺仪 ±250°/s
    data = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, GYRO_CONFIG_REG, 1, &data, 1, 100);
    // 加速度计 ±2g
    data = 0x00;
    HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDR, ACCEL_CONFIG_REG, 1, &data, 1, 100);
    return HAL_OK;
}

/* 读取所有传感器数据 */
void MPU6050_Read_All(MPU6050_Data* data)
{
    uint8_t buf[14];
    // 一次性读取 加速度(6)+温度(2)+陀螺仪(6) 共14字节
    HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDR, 0x3B, 1, buf, 14, 100);
    // 加速度
    data->Ax = (int16_t)(buf[0] << 8 | buf[1]) / 16384.0f;
    data->Ay = (int16_t)(buf[2] << 8 | buf[3]) / 16384.0f;
    data->Az = (int16_t)(buf[4] << 8 | buf[5]) / 16384.0f;
    // 温度
    data->Temperature = ((int16_t)(buf[6] << 8 | buf[7])) / 340.0f + 36.53f;
    // 陀螺仪
    data->Gx = (int16_t)(buf[8] << 8 | buf[9]) / 131.0f;
    data->Gy = (int16_t)(buf[10] << 8 | buf[11]) / 131.0f;
    data->Gz = (int16_t)(buf[12] << 8 | buf[13]) / 131.0f;
}
