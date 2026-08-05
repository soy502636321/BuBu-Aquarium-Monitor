#ifndef VALVE_CONTEXT_HPP
#define VALVE_CONTEXT_HPP

#include <cstdint>
#include <vector>
#include <cstdio>
#include <memory>

#include "Device.hpp"

class DataPacket {
private:
    uint8_t* m_raw = nullptr;
    uint16_t m_length = 0;        // 记录数据长度
    uint16_t m_capacity = 0;      // 记录缓冲区容量（可选）
    void* m_data = nullptr;

public:
    // -------- 构造函数 --------
    DataPacket() = default;

    DataPacket(uint8_t* raw, uint16_t length)
          : m_raw(raw), m_length(length) {}


    // -------- 清空 --------
    void clear() {
        // m_data.clear();
    }

    uint8_t* getRaw() const { return m_raw; }
    uint16_t getLength() const { return m_length; }
    uint16_t getCapacity() const { return m_capacity; }

    template<typename T>
    void setData(T* ptr) {
        m_data = ptr;
    }

};


// ==================== 上下文 ====================
struct DataContext {
	DataPacket packet;  // 引用 Packet
    bool aborted;

    DataContext() = default;

    DataContext(uint8_t* raw, uint16_t length)
        : packet(raw, length), aborted(false) {}

};
#endif // VALVE_CONTEXT_HPP
