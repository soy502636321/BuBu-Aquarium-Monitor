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
    void* m_data = nullptr;

public:
    // -------- 构造函数 --------
    DataPacket() {}

    // -------- 清空 --------
    void clear() {
        // m_data.clear();
    }

    void* getData() const {
        return m_data;
    }

    template<typename T>
    void setData(T* ptr) {
        m_data = ptr;
    }

    uint8_t* getRaw() const {
        return m_raw;
    }

};


// ==================== 上下文 ====================
struct DataContext {
	DataPacket* packet;  // 引用 Packet
    bool aborted;
};
#endif // VALVE_CONTEXT_HPP
