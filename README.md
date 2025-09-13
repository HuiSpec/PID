# 项目概述

这是一个基于 STM32F103C8T6 微控制器的嵌入式项目，主要功能包括：

*   **PID 控制**: 实现了一个 PID 控制器，用于控制电机或其他执行器。
*   **按键输入**: 使用 `multi_button` 库处理多个按键的输入，包括单击、双击、长按等事件。
*   **OLED 显示**: 驱动 OLED 屏幕，用于显示信息、菜单和图形。
*   **编码器输入**: 读取旋转编码器的输入，用于编码电机测速。
*   **MPU6050 传感器**: 读取 MPU6050 传感器的数据，包括加速度和角速度。
*   **LED 控制**: 控制 LED 灯的状态。

项目使用 STM32CubeMX 生成基础代码框架，并使用 HAL 库进行开发。构建系统支持 Makefile 和 PlatformIO。

# 构建和运行

## 先决条件

*   安装 ARM GCC 工具链 (用于 Makefile) 或 PlatformIO (用于 PlatformIO)。
*   安装 OpenOCD 或 ST-Link 工具用于烧录和调试。

## 使用 Makefile 构建

1.  打开终端并导航到项目根目录。
2.  运行 `make` 命令进行编译。
3.  编译生成的文件位于 `build/` 目录下，包括 `.elf`, `.hex`, 和 `.bin` 文件。

## 使用 PlatformIO 构建和烧录

1.  确保已安装 PlatformIO。
2.  打开终端并导航到项目根目录。
3.  运行 `pio run` 进行编译。
4.  运行 `pio run --target upload` 将程序烧录到 STM32 微控制器。

## 调试

*   可以使用 `pio run --target monitor` 启动串口监视器查看调试信息。
*   使用 GDB 和 OpenOCD 进行源码级调试。

# 开发约定

*   **代码结构**: 代码按照功能模块组织在 `User/` 目录下，每个模块包含 `Inc/` 和 `Src/` 子目录。
*   **命名规范**: 使用小写和下划线分隔符命名文件和函数。
*   **注释**: 使用 Doxygen 风格的注释。
*   **HAL 库**: 项目基于 STM32 HAL 库开发。
