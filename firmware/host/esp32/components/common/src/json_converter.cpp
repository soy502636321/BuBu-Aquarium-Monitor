/*
 * json_converter.cpp
 *
 *  Created on: 2026年7月18日
 *      Author: Hu
 */
// json_converter.cpp
#include "json_converter.h"
#include "cJSON.h"
#include "esp_log.h"

static constexpr const char* TAG = "JsonConverter";

// ============================================================================
// 序列化：SensorData → JSON
// ============================================================================

std::string JsonConverter::sensorDataToJson(const DeviceData& data) {
    cJSON* root = cJSON_CreateObject();
    if (!root) {
        ESP_LOGE(TAG, "Failed to create JSON object");
        return "";
    }
    
    // -------- 设备信息 --------
    cJSON_AddStringToObject(root, "device_id", data.device_id.c_str());
    cJSON_AddNumberToObject(root, "timestamp", data.timestamp);
    
    // -------- 序列化 --------
    char* json_str = cJSON_PrintUnformatted(root);
    std::string result(json_str ? json_str : "");
    
    cJSON_free(json_str);
    cJSON_Delete(root);
    
    return result;
}

// ============================================================================
// 序列化：HeartbeatData → JSON
// ============================================================================

/*
std::string JsonConverter::heartbeatToJson(const HeartbeatData& data) {
    cJSON* root = cJSON_CreateObject();
    if (!root) return "";
    
    cJSON_AddStringToObject(root, "device_id", data.device_id.c_str());
    cJSON_AddNumberToObject(root, "timestamp", data.timestamp);
    cJSON_AddNumberToObject(root, "battery", data.battery);
    cJSON_AddNumberToObject(root, "rssi", data.rssi);
    cJSON_AddNumberToObject(root, "uptime", data.uptime);
    cJSON_AddNumberToObject(root, "free_heap", data.free_heap);
    cJSON_AddNumberToObject(root, "wifi_status", data.wifi_status);
    cJSON_AddNumberToObject(root, "mqtt_status", static_cast<uint8_t>(data.mqtt_state));
    
    char* json_str = cJSON_PrintUnformatted(root);
    std::string result(json_str ? json_str : "");
    
    cJSON_free(json_str);
    cJSON_Delete(root);
    
    return result;
}
*/
// ============================================================================
// 序列化：CommandResponse → JSON
// ============================================================================

/*
std::string JsonConverter::responseToJson(const CommandResponse& data) {
    cJSON* root = cJSON_CreateObject();
    if (!root) return "";
    
    cJSON_AddStringToObject(root, "command", data.command.c_str());
    cJSON_AddStringToObject(root, "status", data.status.c_str());
    cJSON_AddStringToObject(root, "message", data.message.c_str());
    cJSON_AddNumberToObject(root, "timestamp", data.timestamp);
    
    char* json_str = cJSON_PrintUnformatted(root);
    std::string result(json_str ? json_str : "");
    
    cJSON_free(json_str);
    cJSON_Delete(root);
    
    return result;
}
*/

// ============================================================================
// 反序列化：JSON → ControlCommand
// ============================================================================

bool JsonConverter::jsonToControlCommand(const std::string& json, DeviceControlCommand& cmd) {
    cJSON* root = cJSON_Parse(json.c_str());
    if (!root) {
        ESP_LOGE(TAG, "Failed to parse JSON: %s", json.c_str());
        return false;
    }
    
    // -------- 必填：command --------
    cJSON* cmdItem = cJSON_GetObjectItem(root, "command");
    if (!cJSON_IsString(cmdItem) || !cmdItem->valuestring) {
        ESP_LOGE(TAG, "Missing or invalid 'command' field");
        cJSON_Delete(root);
        return false;
    }
    cmd.command = cmdItem->valuestring;
    
    // -------- 可选字段 --------
    cJSON* targetItem = cJSON_GetObjectItem(root, "target");
    if (cJSON_IsString(targetItem) && targetItem->valuestring) {
        cmd.target = targetItem->valuestring;
    }
    
    cJSON* tsItem = cJSON_GetObjectItem(root, "timestamp");
    if (cJSON_IsNumber(tsItem)) {
        cmd.timestamp = static_cast<uint64_t>(tsItem->valuedouble);
    }
    
    cJSON* valItem = cJSON_GetObjectItem(root, "value");
    if (cJSON_IsNumber(valItem)) {
        cmd.value = static_cast<float>(valItem->valuedouble);
    }
    
    cJSON* paramItem = cJSON_GetObjectItem(root, "params");
    if (cJSON_IsString(paramItem) && paramItem->valuestring) {
        cmd.params = paramItem->valuestring;
    }
    
    cJSON_Delete(root);
    
    ESP_LOGD(TAG, "Parsed command: %s", cmd.command.c_str());
    return true;
}

// ============================================================================
// 反序列化：JSON → SensorConfig（用于配置下发）
// ============================================================================

bool JsonConverter::jsonToSensorConfig(const std::string& json, DeviceData& data) {
    cJSON* root = cJSON_Parse(json.c_str());
    if (!root) {
        ESP_LOGE(TAG, "Failed to parse config JSON");
        return false;
    }
    
    // 逐个字段提取（不存在则保持默认值）
    cJSON* item = nullptr;
    
    item = cJSON_GetObjectItem(root, "sample_interval");
    if (cJSON_IsNumber(item)) {
        // 可以添加到 SensorData 结构体中
        // data.sample_interval = item->valueint;
    }
    
    item = cJSON_GetObjectItem(root, "alert_threshold");
    if (cJSON_IsNumber(item)) {
        // data.alert_threshold = item->valuedouble;
    }
    
    cJSON_Delete(root);
    return true;
}



