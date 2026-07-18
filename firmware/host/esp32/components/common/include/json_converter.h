/*
 * json_converter.h
 *
 *  Created on: 2026年7月18日
 *      Author: Hu
 */

#ifndef COMPONENTS_COMMON_INCLUDE_JSON_CONVERTER_H_
#define COMPONENTS_COMMON_INCLUDE_JSON_CONVERTER_H_

// json_converter.h
#pragma once

#include "data_types.h"
#include <string>

/**
 * @brief JSON 序列化/反序列化工具
 * 
 * 职责：结构体 ↔ JSON 字符串双向转换
 */
class JsonConverter {
public:
    // ====== 序列化：结构体 → JSON（上报） ======
    
    /**
     * @brief 将传感器数据转换为 JSON 字符串
     */
    static std::string sensorDataToJson(const DeviceData& data);
    
    /**
     * @brief 将心跳数据转换为 JSON 字符串
     */
    //static std::string heartbeatToJson(const HeartbeatData& data);
    
    /**
     * @brief 将命令响应转换为 JSON 字符串
     */
    //static std::string responseToJson(const CommandResponse& data);
    
    // ====== 反序列化：JSON → 结构体（接收） ======
    
    /**
     * @brief 将 JSON 字符串解析为控制命令
     * @param json 输入JSON字符串
     * @param cmd 输出：解析后的命令
     * @return true=解析成功, false=解析失败
     */
    static bool jsonToControlCommand(const std::string& json, DeviceControlCommand& cmd);
    
    /**
     * @brief 将 JSON 字符串解析为传感器数据（用于云端下发的配置）
     */
    static bool jsonToSensorConfig(const std::string& json, DeviceData& data);

private:
    // 内部辅助函数（不对外暴露）
    static std::string buildBaseJson(const std::string& deviceId, uint64_t timestamp);
};

#endif /* COMPONENTS_COMMON_INCLUDE_JSON_CONVERTER_H_ */
