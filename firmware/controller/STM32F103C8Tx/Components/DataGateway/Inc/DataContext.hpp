#ifndef VALVE_CONTEXT_HPP
#define VALVE_CONTEXT_HPP

#include <cstdint>
#include <vector>
#include <cstdio>
#include <memory>

#include "Device.hpp"

class DataPacket {
private:
    std::vector<uint8_t> m_payload;
    uint16_t m_length = 0;        // 记录数据长度
    uint16_t m_capacity = 0;      // 记录缓冲区容量（可选）
    IDeviceData* m_data = nullptr;

public:
    // -------- 构造函数 --------
    DataPacket() = default;

    DataPacket(std::vector<uint8_t> payload, uint16_t length)
          : m_payload(payload), m_length(length) {}


    // -------- 清空 --------
    void clear() {
        // m_data.clear();
    }

    std::vector<uint8_t> getPayload() const { return m_payload; }
    uint16_t getLength() const { return m_length; }
    uint16_t getCapacity() const { return m_capacity; }

	template<typename T>
	bool isType() const {
        return m_data ? m_data->isType<T>() : false;
	}

    uint32_t getTypeId() {
        return m_data ? m_data->getTypeId() : 0;
    }

    template<typename T>
    T* as() {
        return static_cast<T*>(m_data);
    }

    void setData(IDeviceData* ptr) {
        m_data = ptr;
    }

    template<typename T>
    void setData(IDeviceData* ptr) {
        m_data = ptr;
    }

    IDeviceData* getData() {
        return m_data;
    }

    void setPayload(const std::vector<uint8_t> & vector) {
        m_payload = vector;
    }
};

// ==================== 上下文 ====================
struct DataContext {
	DataPacket packet;  // 引用 Packet
    bool aborted;

    DataContext() = default;

    DataContext(std::vector<uint8_t>* payload, uint16_t length): packet(*payload, length), aborted(false) {}

    uint32_t getTypeId() {
        return packet.getTypeId();
    }

    // ========== 获取数据 ==========
    IDeviceData* getData() {
        return packet.getData();
    }

    // ========== 类型判断 ==========
    template<typename T>
    bool isType() const {
        return packet.isType<T>();
    }

    // ========== 类型转换（不安全，需先 isType 检查） ==========
    template<typename T>
    T* as() {
        return packet.as<T>();
    }

    std::vector<uint8_t> getPayload() const {
        return packet.getPayload();
    }

    void setPayload(const std::vector<uint8_t>& payload) {
        packet.setPayload(payload);
    }
};
#endif // VALVE_CONTEXT_HPP
