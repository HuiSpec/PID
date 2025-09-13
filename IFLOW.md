# 项目概述

这是一个基于 STM32F103C8T6 微控制器的嵌入式项目，主要功能包括：

*   **PID 控制**: 实现了一个 PID 控制器，用于精确控制电机速度。
*   **编码器电机驱动**: 利用定时器和编码器接口实现对电机的闭环速度控制。
*   **按键输入**: 集成了按键处理模块，用于用户交互。
*   **OLED 显示**: 使用 OLED 屏幕提供图形用户界面 (GUI) 和信息显示。
*   **MPU6050 传感器**: 集成了 MPU6050 陀螺仪和加速度计模块，用于姿态检测。
*   **LED 指示**: 使用 LED 作为状态指示灯。

## 目录结构

```
.
├── Core/                     # STM32CubeMX 生成的核心代码
│   ├── Inc/                  # 核心头文件
│   └── Src/                  # 核心源文件
├── Drivers/                  # STM32 HAL 库和 CMSIS
│   ├── CMSIS/                # ARM CMSIS
│   └── STM32F1xx_HAL_Driver/ # STM32F1 系列 HAL 驱动
├── PCB/                      # PCB 设计文件 (KiCad)
├── User/                     # 用户代码
│   ├── BUTTON/               # 按键驱动
│   ├── ENCODER/              # 编码器电机驱动
│   ├── GUI/                  # OLED 图形用户界面
│   ├── LED/                  # LED 驱动
│   ├── MPU6050/              # MPU6050 传感器驱动
│   ├── OLED/                 # OLED 屏幕驱动
│   └── PID/                  # PID 控制算法
├── Makefile                  # Makefile 构建脚本
├── platformio.ini            # PlatformIO 项目配置
├── STM32F103C8Tx_FLASH.ld    # 链接脚本
└── startup_stm32f103xb.s     # 启动文件
```

## 核心技术

*   **微控制器**: STM32F103C8T6 (ARM Cortex-M3)
*   **开发环境**: 
    *   PlatformIO (主要)
    *   Makefile (备用)
*   **SDK**: STM32Cube HAL 库
*   **编程语言**: C
*   **构建系统**: PlatformIO / GNU Make

## 开发约定

*   **代码结构**: 项目遵循模块化设计，每个功能模块（如 ENCODER, PID, OLED 等）都有独立的 `Inc` 和 `Src` 目录。
*   **命名规范**: 使用下划线分隔的蛇形命名法 (snake_case)。
*   **注释风格**: 使用 Doxygen 风格的注释。

# 构建和运行

## 先决条件

*   安装 [PlatformIO Core](https://docs.platformio.org/en/latest/core/installation.html) 或 [GNU Arm Embedded Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm) 和 Make。

## 使用 PlatformIO 构建和烧录

```bash
# 构建项目
pio run

# 烧录固件到 STM32
pio run --target upload

# 监视串口输出
pio device monitor
```

## 使用 Makefile 构建

```bash
# 构建项目
make all

# 清理构建文件
make clean
```

烧录和调试需要额外的工具（如 ST-Link）和相应的命令，具体取决于你的硬件设置。

# 开发说明

## 主程序流程

1.  系统初始化 (`HAL_Init`, `SystemClock_Config`)。
2.  外设初始化 (`MX_GPIO_Init`, `MX_I2C1_Init`, `MX_TIMx_Init` 等)。
3.  用户模块初始化 (`init_buttons`, `OLED_Init`, `Encoder_Motor_Init`)。
4.  显示启动 Logo。
5.  进入主循环，执行菜单逻辑 (`main_menu`)，根据选择进入不同的功能子程序 (`text_function`, `pid_function`, `mpu_function`)。

## 关键模块

*   **PID 控制 (`User/PID/`)**: 实现了标准的 PID 算法，包含防风和微分滤波。
*   **编码器电机 (`User/ENCODER/`)**: 使用 TIM3 作为编码器接口读取速度，TIM1 生成 PWM 控制电机，TIM2 定时中断进行速度更新和 PID 计算。
*   **GUI (`User/GUI/`)**: 基于 OLED 驱动实现了一个简单的菜单和文本显示界面。
*   **MPU6050 (`User/MPU6050/`)**: 通过 I2C 接口读取 MPU6050 的数据。

## 配置文件

*   `platformio.ini`: PlatformIO 项目配置，定义了开发板、框架、构建标志和源文件过滤器。
*   `Makefile`: GNU Make 构建脚本，由 STM32CubeMX 生成。
*   `STM32F103C8Tx_FLASH.ld`: 链接脚本，定义了代码在 Flash 中的内存布局。