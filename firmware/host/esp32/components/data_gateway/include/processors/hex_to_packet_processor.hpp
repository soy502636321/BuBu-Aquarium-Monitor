#ifndef HEX_TO_PACKET_PROCESSOR
#define HEX_TO_PACKET_PROCESSOR
/**
 * 负责把原始的raw解析成具体的数据给下一步
 */
#include "data_processor.hpp"
#include "device.hpp"

#include "esp_log.h"
#include "hex.hpp"
#include "value_type.hpp"

#define TAG "BuBu-Aquarium-Monitor[hex_to_packet_processor]"

// 帧头
#define DATA_Header_POS 0
#define DATA_Header_LEN 2
// 协议版本
#define DATA_VERSION_POS 2
#define DATA_VERSION_LEN 1


class HexToPacketProcessor : public IDataProcessor
{

public:

	bool execute(DataContext& ctx) override
	{
		ESP_LOGI(TAG, "HexToPacketProcessor");

		uint8_t* payload = ctx.packet.payload.data();
		size_t payload_len = ctx.packet.payload.size();
		DataType data_type = static_cast<DataType>(payload[3]);  // 消息类型
		uint16_t data_len = (payload[4] << 8) | payload[5]; // 数据长度
		uint8_t data_count = payload[6]; // 数据数量
		ESP_LOGI("Parser", "📦 DataType: %d (0x%02X), DataLen: %d bytes", static_cast<int>(data_type), payload[3], data_len);

		// 解析T-TLV格式数据
		uint8_t point_pos = 7;
		for (uint8_t i = 0; i < data_count; i++) {
			DataPointType point_type = static_cast<DataPointType>(payload[point_pos]);  // 属性类型
			ValueType point_value_type = static_cast<ValueType>(payload[point_pos + 1]); //属性值类型
			uint8_t point_value_len = payload[point_pos + 2];
			auto point_value = Utils::HEX::hex_to_value(point_value_type, payload + (point_pos + 2), point_value_len);

			DataPoint point;
			point.type = point_type;
			point.unit = "单位";
			point.setValue(point_value);

			ESP_LOGI(TAG, "=== DataPoint Created ===");
			ESP_LOGI(TAG, "  Type: %d", static_cast<int>(point.type));
			ESP_LOGI(TAG, "  Unit: %s", point.unit.c_str());
			std::visit([](auto&& arg) {
				using T = std::decay_t<decltype(arg)>;
				if constexpr (std::is_same_v<T, std::string>) {
					ESP_LOGI("TAG", "Value: %s", arg.c_str());  // ✅ string 用 c_str()
				} else {
					ESP_LOGI("TAG", "Value: %s", std::to_string(arg).c_str());
				}
			}, point.value);
			point_pos = point_pos + 3 + point_value_len; //移位下一个属性
		}
		return true;
	}

};
#endif
