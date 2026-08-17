#ifndef VALVE_CONTEXT_HPP
#define VALVE_CONTEXT_HPP

#include "Device.hpp"
#include <string.h>

#define MAX_PAYLOAD_SIZE 64

using DataVariant = std::variant<std::monostate, DeviceRecord, DeviceSwitch, DevicePwm, DeviceSetup, DeviceCollection>;

class DataPacket {
private:
    uint8_t m_payload[MAX_PAYLOAD_SIZE];
    size_t m_length = 0;        // 记录数据长度
    uint16_t m_capacity = 0;      // 记录缓冲区容量（可选）
    DataVariant m_data;
    uint16_t m_version = 1;

public:
    // -------- 构造函数 --------
    DataPacket() : m_length(0), m_version(1) {}

    // ✅ 从原始数据构造（拷贝）
    DataPacket(const uint8_t* data, size_t len) {
        setPayload(data, len);
    }


    // -------- 清空 --------
    void clear() {
        m_length = 0;
        m_capacity = 0;
        m_version = 1;
        // 如果需要重置 IDeviceData
        m_data = std::monostate{};
        // 清空 payload（可选，安全起见）
        memset(m_payload, 0, MAX_PAYLOAD_SIZE);
    }

    const uint8_t *getPayload() const { return m_payload; }
    size_t getLength() const { return m_length; }
    uint16_t getCapacity() const { return m_capacity; }

	template<typename T>
	bool isType() const {
        // 检查是否为空
        if (std::holds_alternative<std::monostate>(m_data)) {
            return false;
        }
        // 检查是否为 T 类型
        return std::holds_alternative<T>(m_data);
	}

    uint32_t getTypeId() const {
        return std::visit([](auto& d) -> uint32_t {
            using T = std::decay_t<decltype(d)>;
            if constexpr (std::is_same_v<T, std::monostate>) {
                return 0;  // ✅ 空状态返回 0
            } else {
                return d.getTypeId();
            }
        }, m_data);
    }

    template<typename T>
    void setData(const T& ptr) {
        m_data = ptr;
    }

    template<typename T>
    T* getData() {
        // 检查是否为空
        if (std::holds_alternative<std::monostate>(m_data)) {
            return nullptr;
        }
        // 检查是否为 T 类型
        if (!std::holds_alternative<T>(m_data)) {
            return nullptr;
        }
        return std::get_if<T>(&m_data);
    }

    void setPayload(const uint8_t* data, size_t len) {
        // ========== 日志追踪 ==========
        static uint32_t call_count = 0;
        call_count++;

        printf("[setPayload #%u] === START ===\n", call_count);
        printf("  data = 0x%p, len = %u, MAX_PAYLOAD_SIZE = %d\n",
               data, (unsigned int) len, MAX_PAYLOAD_SIZE);
        fflush(stdout);

        // ========== 检查 data 指针 ==========
        if (data == nullptr) {
            printf("[setPayload #%u] ❌ ERROR: data is NULL!\n", call_count);
            m_length = 0;
            fflush(stdout);
            return;
        }

        // ========== 检查 len ==========
        if (len == 0) {
            printf("[setPayload #%u] ⚠️ WARNING: len is 0, clearing payload\n", call_count);
            m_length = 0;
            fflush(stdout);
            return;
        }

        // ========== 检查 len 是否越界 ==========
        if (len > MAX_PAYLOAD_SIZE) {
            printf("[setPayload #%u] ❌❌❌ CRITICAL: len (%u) > MAX_PAYLOAD_SIZE (%d) !!!\n",
                   call_count, (unsigned int)len, MAX_PAYLOAD_SIZE);
            printf("  This will cause buffer overflow! Data source: 0x%p\n", data);
            printf("  Printing first 32 bytes of source data:\n  ");
            for (size_t i = 0; i < 32 && i < len; i++) {
                printf("%02X ", data[i]);
            }
            printf("\n");
            fflush(stdout);

            // ========== 调试断点：程序停在这里 ==========
            // 如果越界，停住方便调试
            while (1) {
                // 闪烁 LED 指示错误
                for (volatile int i = 0; i < 1000000; i++);
            }
        }

        // ========== 打印数据内容（前 16 字节） ==========
        printf("[setPayload #%u] Copying %u bytes from 0x%p to m_payload\n",
               call_count, (unsigned int
                   )len, data);
        printf("  Data hex: ");
        for (size_t i = 0; i < len && i < 16; i++) {
            printf("%02X ", data[i]);
        }
        if (len > 16) printf("... (%u bytes total)", (unsigned int)len);
        printf("\n");
        fflush(stdout);

        // ========== 执行拷贝 ==========
        memcpy(m_payload, data, len);
        m_length = len;

        // ========== 验证拷贝结果 ==========
        printf("[setPayload #%u] ✅ Copy successful, m_length = %u\n",
               call_count, (unsigned int)m_length);
        fflush(stdout);
    }

    void setLength(uint16_t length) {
        m_length = length;
    }

    void setVersion(uint8_t version) {
        m_version = version;
    }
    uint8_t getVersion() const {
        return m_version;
    }
    void setDataType(DeviceDataType data_type) {
        std::visit([data_type](auto& d) {
            using T = std::decay_t<decltype(d)>;
            if constexpr (!std::is_same_v<T, std::monostate>) {
                d.setDataType(data_type);
            }
        }, m_data);
    }

    bool isValid() const {
        return m_length > 0;
    }
};

// ==================== 上下文 ====================
struct DataContext {
	DataPacket packet;  // 引用 Packet
    bool aborted;

    DataContext() = default;

    DataContext(const uint8_t* payload, uint16_t length): packet(payload, length), aborted(false) {}

    uint32_t getTypeId() {
        return packet.getTypeId();
    }

    uint32_t getDataTypeId() {
        return packet.getTypeId();
    }

    // ========== 获取数据 ==========
    template<typename T>
    T* getData() {
        return packet.getData<T>();
    }

    // ========== 类型判断 ==========
    template<typename T>
    bool isType() const {
        return packet.isType<T>();
    }

    // ========== 类型转换（不安全，需先 isType 检查） ==========
    // template<typename T>
    // T& as() {
    //     return packet.as<T&>();
    // }

    const uint8_t* getPayload() const {
        return packet.getPayload();
    }

    void setPayload(const uint8_t& payload, size_t length) {
        packet.setPayload(&payload, length);
    }

    template<typename T>
    void setData(T ptr) {
        packet.setData(ptr);
    }

    void reset() {
        packet.clear();      // 清空 payload
        aborted = false;
    }

};
#endif // VALVE_CONTEXT_HPP
