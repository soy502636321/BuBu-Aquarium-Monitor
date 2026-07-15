# 咘咘智能鱼缸 [BuBu Aquarium Monitor]

BuBu Aquarium Monitor 是一套模块化智能鱼缸控制系统，采用 Host + Controller 分层设计。STM32 控制模块负责传感器数据采集、设备驱动及执行控制；主机模块负责数据管理、GUI显示、网络通信和用户交互。系统支持温度、pH、TDS、溶氧等多种水质参数监测，并支持水泵、灯光、加热、过滤等设备的自动/手动控制。通过 UART、RS485、WiFi 等通信方式实现设备互联，可扩展至 ESP32、RK3506/Linux 等不同硬件平台。

暂时只开发ESP32-S3

## 系统架构

```
                                     ┌───────────────────┐
                                     │	 Android APP     │
                                     │  	(Kotlin)     │
                                     └─────────┬─────────┘
                                               │
                                               │ WiFi / MQTT
                                               │
┌───────────────┐   蓝牙/UART   ┌──────────────┼──────────────┐
│               │◄─────────────►│                             │
│   STM32       │               │ ESP32/Linux                 │
│ (采集控制模块)│                │ (WiFi网关)                   │
│               │               │                             │
└───────────────┘               └──────────────┼──────────────┘
       │                                       │
       │                                       │ WiFi / MQTT
       │                                       │
       │                              ┌────────┴────────┐
       │                              │                 │
       │                              │ Home Assistant  │
       │                              │ (智能家居平台)   │
       │                              │                 │
       │                              └─────────────────┘
       │
	   │			├──水位传感器（型号：）
	   │			├──水温传感器（型号：）
	   │			├──水质传感器（型号：）
       ├──├传感器├──├──PH传感器（型号：）
	   │			├──含氧量传感器（型号：）
	   │			├──水流传感器（型号：）
	   │			├──TDS传感器（型号：）
	   │			├──盐度传感器（型号：）
	   │			├──其他（待定，先列举出来，不一定能对接）
	   │
	   │			├──灯光
	   │			├──循环水泵
	   │	   		├──排污
       ├──├继电器├──├──排水/排水
       │			├──加热/制冷
	   │			├──制氧
	   │			├──自动喂食
	   │
	   │		 ├──UV杀菌（如果支持）
       ├──├PWM├──├──制氧（如果支持）
				 ├──循环水泵（如果支持）
```
## 项目结构

```
BuBu-Aquarium-Monitor
│
├── firmware
│   │
│   ├── host                         # 主机端
│   │   │
│   │   ├── esp32                     # ESP32-S3主机
│   │   │   │
│   │   │   ├── main
│   │   │   │   ├── main.cpp
│   │   │   │   └── app_main.cpp
│   │   │   │
│   │   │   ├── components
│   │   │   │   │
│   │   │   │   ├── ui                # LVGL + EEZ Studio
│   │   │   │   │   ├── screens
│   │   │   │   │   ├── widgets
│   │   │   │   │   ├── generated     # EEZ生成代码
│   │   │   │   │   └── ui_manager
│   │   │   │   │
│   │   │   │   ├── display           # LCD驱动
│   │   │   │   │   ├── lcd_driver
│   │   │   │   │   └── touch
│   │   │   │   │
│   │   │   │   ├── communication      # 与控制器通信
│   │   │   │   │   ├── uart
│   │   │   │   │   ├── rs485
│   │   │   │   │   └── ble
│   │   │   │   │
│   │   │   │   ├── device             # 设备管理
│   │   │   │   │   ├── controller_manager
│   │   │   │   │   ├── sensor_manager
│   │   │   │   │   └── alarm_manager
│   │   │   │   │
│   │   │   │   ├── network             # 网络
│   │   │   │   │   ├── wifi
│   │   │   │   │   ├── mqtt
│   │   │   │   │   └── http
│   │   │   │   │
│   │   │   │   └── storage             # 本地存储
│   │   │   │       ├── nvs
│   │   │   │       └── config
│   │   │   │
│   │   │   └── partitions.csv
│   │   │
│   │   ├── rk3506                     # 未来Linux主机
│   │   │   ├── lvgl
│   │   │   ├── ui
│   │   │   ├── network
│   │   │   └── app
│   │   │
│   │   └── linux                      # 可选
│   │       └── lvgl_app
│   │
│   │
│   └── controller                     # 控制采集端
│       │
│       └── stm32                      # STM32控制器
│           │
│           ├── Core                   # CubeMX核心
│           │   ├── Inc
│           │   └── Src
│           │
│           ├── Drivers                # ST HAL
│           │   ├── CMSIS
│           │   └── STM32_HAL
│           │
│           ├── BSP                    # 板级驱动
│           │   ├── bsp_gpio
│           │   ├── bsp_adc
│           │   ├── bsp_i2c
│           │   ├── bsp_uart
│           │   └── bsp_pwm
│           │
│           ├── Components             # 功能组件
│           │   │
│           │   ├── sensors             # 传感器
│           │   │   ├── temperature
│           │   │   ├── ph
│           │   │   ├── tds
│           │   │   ├── do
│           │   │   └── water_level
│           │   │
│           │   ├── actuator            # 执行器
│           │   │   ├── pump
│           │   │   ├── heater
│           │   │   ├── light
│           │   │   └── feeder
│           │   │
│           │   ├── communication        # 通信
│           │   │   ├── uart
│           │   │   ├── rs485
│           │   │   └── esp01
│           │   │
│           │   ├── protocol             # 协议
│           │   │   ├── packet.c
│           │   │   └── command.c
│           │   │
│           │   └── storage
│           │       └── eeprom
│           │
│           ├── App                    # 应用层
│           │   ├── app_main.c
│           │   ├── app_sensor.c
│           │   ├── app_control.c
│           │   └── app_task.c
│           │
│           ├── MDK-ARM
│           └── STM32CubeIDE
│
│
├── common                             # 主机和控制器共享
│   │
│   ├── protocol                       # 通信协议
│   │   ├── packet.h
│   │   ├── command.h
│   │   └── sensor_data.h
│   │
│   ├── data_model                     # 数据模型
│   │   ├── aquarium.h
│   │   └── device.h
│   │
│   └── version.h
│
├── tools
│   ├── protocol_test
│   └── firmware_tool
│
└── docs
    ├── architecture.md
    ├── protocol.md
    └── hardware.md

    AI生成，后面调整
```

## License

MIT License
