/*
 * protocol_parser.cpp
 *
 *  Created on: 2026年7月23日
 *      Author: Hu
 */

// components/data_gateway/src/protocol_parser.cpp
#include "protocol_parser.hpp"
#include "esp_log.h"
#include <cstring>
#include <ctime>

static const char* TAG = "ProtocolParser";

void ProtocolParser::parseRawData(const std::vector<uint8_t>& raw_data) {
    if (raw_data.empty()) {
        return;
    }
    
    // 追加到缓冲区
    m_buffer.insert(m_buffer.end(), raw_data.begin(), raw_data.end());
    
    // 循环解析完整数据帧
    while (m_buffer.size() >= MIN_FRAME_SIZE) {
        size_t start_pos = 0;
        if (!findFrameHeader(start_pos)) {
            // 没有找到帧头，清空缓冲区（或保留部分数据）
            if (m_buffer.size() > 1024) {
                // 缓冲区过大，清空防止溢出
                m_buffer.clear();
                ESP_LOGW(TAG, "Buffer overflow, cleared");
            }
            break;
        }
        
        // 如果帧头不在起始位置，移除前面的数据
        if (start_pos > 0) {
            m_buffer.erase(m_buffer.begin(), m_buffer.begin() + start_pos);
        }
        
        // 检查是否有足够的数据（至少需要知道长度字段）
        if (m_buffer.size() < 4) {
            break;
        }
        
        // 协议格式: AA 55 数据长度(2字节) 数据... 校验(1字节)
        uint16_t data_len = (m_buffer[2] << 8) | m_buffer[3];
        size_t frame_len = 4 + data_len + 1;  // 帧头2 + 长度2 + 数据 + 校验1
        
        if (m_buffer.size() < frame_len) {
            // 数据不完整，等待更多数据
            break;
        }
        
        // 验证校验和
        std::vector<uint8_t> frame(m_buffer.begin(), m_buffer.begin() + frame_len);
        if (!verifyChecksum(frame)) {
            ESP_LOGW(TAG, "Checksum verification failed");
            // 跳过第一个字节，继续查找
            m_buffer.erase(m_buffer.begin());
            continue;
        }
        
        // 提取数据部分
        std::vector<uint8_t> data(m_buffer.begin() + 4, m_buffer.begin() + 4 + data_len);
        
        // 解析数据为 DeviceRecord
        DeviceRecord record = parseDataFrame(data);
        
        // 通过回调上报
        if (m_callback) {
            m_callback(record);
        }
        
        // 移除已处理的数据
        m_buffer.erase(m_buffer.begin(), m_buffer.begin() + frame_len);
        
        ESP_LOGD(TAG, "Parsed frame, device: %s, points: %zu", 
                 record.device_id.c_str(), record.points.size());
    }
}

bool ProtocolParser::findFrameHeader(size_t& start_pos) {
    for (size_t i = 0; i + 1 < m_buffer.size(); i++) {
        if (m_buffer[i] == FRAME_HEADER_1 && m_buffer[i + 1] == FRAME_HEADER_2) {
            start_pos = i;
            return true;
        }
    }
    return false;
}

bool ProtocolParser::verifyChecksum(const std::vector<uint8_t>& frame) {
    if (frame.size() < 2) return false;
    
    uint8_t checksum = 0;
    // 对帧头+长度+数据计算校验和（不包括校验字节本身）
    for (size_t i = 0; i < frame.size() - 1; i++) {
        checksum ^= frame[i];
    }
    
    return checksum == frame.back();
}

DeviceRecord ProtocolParser::parseDataFrame(const std::vector<uint8_t>& data) {
    DeviceRecord record;
    record.device_id = "sensor_01";
    record.device_name = "温湿度传感器";
    // record.device_type = device::Type::Sensor; 编译注解
    record.timestamp = getCurrentTimestamp();
    
    // 根据实际协议解析数据
    // 示例格式: [设备ID(16字节)] [温度(float 4字节)] [湿度(float 4字节)]
    // 这里使用简化版本
    
    if (data.size() >= 8) {
        // 假设前4字节是温度，后4字节是湿度
        float temp = 0;
        float hum = 0;
        memcpy(&temp, data.data(), 4);
        memcpy(&hum, data.data() + 4, 4);
        
        DataPoint temp_point;
        //temp_point.type = "temperature";
        temp_point.value = temp;
        temp_point.unit = "°C";
        temp_point.quality = 0;
        record.points.push_back(temp_point);
        
        DataPoint hum_point;
       // hum_point.type = "humidity";
        hum_point.value = hum;
        hum_point.unit = "%";
        hum_point.quality = 0;
        record.points.push_back(hum_point);
        
        ESP_LOGD(TAG, "Parsed: temp=%.2f°C, hum=%.2f%%", temp, hum);
    }
    
    return record;
}

void ProtocolParser::reset() {
    m_buffer.clear();
}

std::string ProtocolParser::getCurrentTimestamp() {
    time_t now = time(nullptr);
    struct tm* tm_info = localtime(&now);
    char buf[20];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm_info);
    return std::string(buf);
}


