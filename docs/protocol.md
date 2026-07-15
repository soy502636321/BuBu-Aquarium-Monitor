# BuBu Aquarium Communication Protocol

**Project:** BuBu Aquarium Monitor  
**Protocol Version:** V1.0  
**Date:** 2026-07  

---

# 1. 系统架构

BuBu 智能鱼缸系统采用 Host + Controller 分层架构。

```
                 Android APP
                      |
                      |
                    MQTT
                      |
                Cloud Server
                      |
                      |
              +----------------+
              |      Host      |
              | ESP32 / RK3506 |
              | Linux Platform |
              +----------------+
                      |
                 UART / BLE
                      |
              +----------------+
              |  Controller    |
              |     STM32      |
              +----------------+
                      |
       --------------------------------
       |              |               |
    传感器采集       PWM控制        开关控制
```

---

# 2. 协议目标

协议用于：

- STM32 Controller 与 Host 通信
- 传感器数据上传
- PWM设备控制
- GPIO开关控制
- MQTT数据转换
- Android APP远程控制


支持：

- UART
- RS485
- BLE
- WiFi Gateway


---

# 3. 数据帧格式

所有设备通信采用统一数据帧：

```
+--------+--------+--------+--------+-------------+--------+
| HEAD   | VER    | TYPE   | LEN    | DATA        | CRC16  |
+--------+--------+--------+--------+-------------+--------+
| 2Byte  | 1Byte  | 1Byte  | 2Byte  | N Byte      | 2Byte  |
+--------+--------+--------+--------+-------------+--------+
```

---

# 4. 字段说明


| 字段 | 长度 | 说明 | 示例 |
|----|----|----|----|
| HEAD | 2Byte | 帧头 | AA 55 |
| VER | 1Byte | 协议版本 | 01 |
| TYPE | 1Byte | 消息类型 | 02 |
| LEN | 2Byte | DATA长度 | 20 |
| DATA | N Byte | 数据内容 | 属性数据 |
| CRC16 | 2Byte | 校验 | CRC16-MODBUS |


---

# 5. HEAD定义


固定：

```
0xAA 0x55
```


作用：

- 数据同步
- 判断数据开始


---

# 6. TYPE消息类型


| TYPE | 名称 | 方向 |
|-|-|-|
|0x01|设备状态上报|STM32 → Host|
|0x02|传感器数据上报|STM32 → Host|
|0x03|控制命令|Host → STM32|
|0x04|控制响应|STM32 → Host|


---

# 7. CRC16校验


采用：

```
CRC16-MODBUS
```


参数：

|项目|值|
|-|-|
|Polynomial|0xA001|
|Initial Value|0xFFFF|
|Input|LSB First|
|Output|Low Byte First|


计算范围：

```
HEAD + VER + TYPE + LEN + DATA
```


不包含：

```
CRC16
```


示例：

发送数据：

```
AA 55 01 02 10 DATA
```


计算：

```
CRC16(
AA 55 01 02 10 DATA
)
```


追加：

```
CRC_LOW CRC_HIGH
```


最终：

```
AA 55 01 02 10 DATA CRC_LOW CRC_HIGH
```


---

# 8. 属性模型 Property


所有传感器和控制参数统一采用属性模型。


格式：

```
+------+-------+------+---------+
| ID   | TYPE  | UNIT | VALUE   |
+------+-------+------+---------+
|2Byte |1Byte  |2Byte |N Byte   |
+------+-------+------+---------+
```


---

# 9. 属性结构


C语言：

```c
typedef struct
{
    uint16_t id;

    uint8_t type;

    uint16_t unit;

    uint8_t value[];

}property_t;
```


---

# 10. 数据类型


|TYPE|说明|长度|
|-|-|-|
|0x01|INT8|1Byte|
|0x02|UINT8|1Byte|
|0x03|INT16|2Byte|
|0x04|UINT16|2Byte|
|0x05|INT32|4Byte|
|0x06|UINT32|4Byte|
|0x07|FLOAT32|4Byte|
|0x08|BOOL|1Byte|


---

# 11. 单位定义


|ID|单位|
|-|-|
|0x0001|℃|
|0x0002|pH|
|0x0003|ppm|
|0x0004|mg/L|
|0x0005|cm|
|0x0006|%|


---

# 12. 传感器数据上报


## TYPE

```
0x02
```


DATA格式：

```
COUNT

Property1

Property2

...

PropertyN
```


---

## 示例


STM32采集：

```
温度:
26.5℃


PH:
7.2


TDS:
350ppm


溶氧:
8.1mg/L
```


发送：

```
COUNT = 4
```


属性1：

```
ID:

0001


TYPE:

FLOAT32


UNIT:

0001


VALUE:

26.5
```


属性2：

```
ID:

0002


TYPE:

FLOAT32


UNIT:

0002


VALUE:

7.2
```


属性3：

```
ID:

0003


TYPE:

UINT32


UNIT:

0003


VALUE:

350
```


属性4：

```
ID:

0004


TYPE:

FLOAT32


UNIT:

0004


VALUE:

8.1
```


一次通信即可上传多个参数。


---

# 13. PWM控制


## CMD

```
0x20
```


格式：

```
+------+------+
| CH   |DUTY |
+------+------+
|1Byte |1Byte|
+------+------+
```


---

## PWM通道


|ID|设备|
|-|-|
|0x01|灯光|
|0x02|水泵|
|0x03|风扇|
|0x04|加热|


---

## 示例

设置灯光50%:


Host发送：

```
TYPE:

03


CMD:

20


CH:

01


DUTY:

50
```


STM32执行：

```c
PWM_LIGHT = 50%;
```


---

# 14. 开关控制


## CMD


```
0x21
```


格式：

```
DEVICE_ID

STATE
```


---

设备：

|ID|设备|
|-|-|
|01|水泵|
|02|灯光|
|03|加热|
|04|过滤|


状态：

|值|状态|
|-|-|
|0|关闭|
|1|打开|


---

## 示例


打开水泵：


```
CMD:

21


DEVICE:

01


STATE:

01
```


STM32：

```c
pump_on();
```


---

# 15. MQTT协议转换


ESP32 Host负责协议转换。


Topic:


```
bubu/device/001/property/report
```


JSON:


```json
{
 "device":"001",

 "properties":[

  {
   "id":1,
   "value":26.5,
   "unit":"℃"
  },


  {
   "id":2,
   "value":7.2,
   "unit":"pH"
  },


  {
   "id":3,
   "value":350,
   "unit":"ppm"
  }

 ]
}
```


---

# 16. APP控制流程


例如APP打开水泵：


```
Android APP

      |

     MQTT

      |

ESP32 Host

      |

Protocol

      |

STM32 Controller

      |

GPIO

      |

Pump
```


MQTT消息：

```json
{
 "device":"001",

 "command":
 {
   "pump":1
 }
}
```


转换：

```
TYPE = 0x03

CMD  = 0x21

DEVICE = 0x01

STATE = 1
```


---

# 17. 扩展能力


支持：

## 传感器

- 温度
- pH
- TDS
- DO
- ORP
- 水位
- 流量


## 控制

- 水泵
- 灯光
- 加热
- 风扇
- 自动投喂


## 主机平台

- ESP32-S3
- RK3506
- Linux


---

# 18. 文件结构


```
common

├── protocol

│   ├── packet.h
│   ├── crc16.c
│   ├── command.h
│   └── sensor.h


└── data_model

    └── property.h
```


---

# 19. 版本记录


|版本|日期|说明|
|-|-|-|
|V1.0|2026-07|初始版本|
