#ifndef DATAGATEWAY_PACKET_V1_ENCODER_RX_VALVE_HPP
#define DATAGATEWAY_PACKET_V1_ENCODER_RX_VALVE_HPP
#include "stdio.h"
#include "value_type.hpp"

#define MAX_FRAME_SIZE 128

class PacketV1EncoderTxValve: public ITxValve {

public:
    bool process(DataContext& ctx) {
        switch (ctx.getTypeId()) {
            case DeviceRecord::TYPE_ID: {
                return processRecord(ctx);
            }
                default:
                break;
        }
        return true;
    }

    // 处理设备监测数据
    bool processRecord(DataContext& ctx) {
        DeviceRecord* record = ctx.getData<DeviceRecord>();
        // const uint8_t* payload[MAX_PAYLOAD_SIZE];  //
        // payload[0] = 0xAA;
        // payload[1] = 0x55;
        // payload[2] = 0x01;
        // payload[3] = 0x01;
        printf("processRecord(DataContext& ctx)\r\n");
        // // 格式头
        // uint8_t data_len = 0;
        // record->forEachDataPoint([&payload, &data_len](size_t idx, const DataPoint& point) {
        //     payload.push_back(static_cast<uint8_t>(point.type));  // Data Type
        //     DataValue value = point.value;
        //     DataValueType value_type = getValueType(value);
        //     size_t value_size = getValueSize(value);
        //     std::vector<uint8_t> value_hex = getValueHex(point.value);
        //
        //     // T-TLV
        //     payload.push_back(0x01);  // 数据类型
        //     payload.push_back(static_cast<uint8_t>(value_type)); //数据值类型
        //     payload.push_back(value_size); //数据值长度
        //     payload.insert(payload.end(), value_hex.begin(), value_hex.end()); // 数据
        //
        //     data_len = data_len + 1 + 1 + 1 + value_size;
        // });
        // // 回填数据长度
        // payload.at(4) = data_len;
        // // 加入CRC16验证
        // uint16_t crc16_verify = CRC16::calc(payload.data(), payload.size());
        // payload.push_back(crc16_verify & 0xFF);          // 低字节在前
        // payload.push_back((crc16_verify >> 8) & 0xFF);
        //
        // ctx.setPayload(payload, data_len);
        return true;
    }
};
#endif //DATAGATEWAY_PACKET_V1_ENCODER_RX_VALVE_HPP