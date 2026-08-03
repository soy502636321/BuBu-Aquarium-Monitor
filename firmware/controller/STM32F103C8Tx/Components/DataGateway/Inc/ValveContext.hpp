#ifndef VALVE_CONTEXT_HPP
#define VALVE_CONTEXT_HPP

#include <cstdint>
#include <vector>
#include <cstdio>
#include <cstring>

namespace DataGateway {

class ValvePacket {
private:
    uint32_t sequence_;
    uint32_t timestamp_;
    std::vector<uint8_t> data;  // [type][value] * N

public:
    // -------- 构造函数 --------
    ValvePacket() : sequence_(0), timestamp_(0) {}

    // -------- 清空 --------
    void clear() {
        data.clear();
    }
};


// ==================== 上下文 ====================
struct ValveContext {
	ValvePacket* packet;  // 引用 Packet
};

} // namespace DataGateway
#endif // VALVE_CONTEXT_HPP
