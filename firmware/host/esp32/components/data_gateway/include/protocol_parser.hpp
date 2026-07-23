/*
 * protocol_parser.hpp
 *
 *  Created on: 2026年7月23日
 *      Author: Hu
 */

#ifndef COMPONENTS_DATA_GATEWAY_INCLUDE_PROTOCOL_PARSER_HPP_
#define COMPONENTS_DATA_GATEWAY_INCLUDE_PROTOCOL_PARSER_HPP_

#include <vector>
#include <string>
#include <functional>
#include <cstdint>
#include "device.hpp"

// 解析完成后的回调（传递给业务层）
using DeviceRecordCallback = std::function<void(const DeviceRecord&)>;

// 协议解析器
class ProtocolParser {
public:
    ProtocolParser() = default;
    ~ProtocolParser() = default;
    
    // 设置解析完成后的回调
    void setRecordCallback(DeviceRecordCallback callback) {
        m_callback = callback;
    }
    
    // 接收原始数据并解析
    void parseRawData(const std::vector<uint8_t>& raw_data);
    
    // 重置解析状态（清空缓冲区）
    void reset();
    
    // 获取当前缓冲区大小
    size_t getBufferSize() const { return m_buffer.size(); }
    
    // 设置协议配置
    void setConfig(const std::string& config) { m_config = config; }
    
private:
    // 解析数据帧为 DeviceRecord
    DeviceRecord parseDataFrame(const std::vector<uint8_t>& data);
    
    // 查找帧头
    bool findFrameHeader(size_t& start_pos);
    
    // 验证校验和
    bool verifyChecksum(const std::vector<uint8_t>& frame);
    
    // 获取当前时间戳
    std::string getCurrentTimestamp();
    
private:
    std::vector<uint8_t> m_buffer;      // 数据缓冲区
    DeviceRecordCallback m_callback;    // 解析完成回调
    std::string m_config;               // 配置信息
    
    // 协议常量（可配置）
    static constexpr uint8_t FRAME_HEADER_1 = 0xAA;
    static constexpr uint8_t FRAME_HEADER_2 = 0x55;
    static constexpr size_t MIN_FRAME_SIZE = 8;  // 最小帧长度
};

#endif /* COMPONENTS_DATA_GATEWAY_INCLUDE_PROTOCOL_PARSER_HPP_ */
