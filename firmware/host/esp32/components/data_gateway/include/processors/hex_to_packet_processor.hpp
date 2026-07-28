#ifndef HEX_TO_PACKET_PROCESSOR
#define HEX_TO_PACKET_PROCESSOR
/**
 * 负责把原始的raw解析成具体的数据给下一步
 */
#include "data_processor.hpp"
#include "device.hpp"

#include "esp_log.h"

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
		uint8_t point_len = 0;

		for (uint8_t i = 0; i < data_count; i++) {
			DataPointType point_type = static_cast<DataPointType>(payload[point_pos]);  // 属性类型
			uint8_t point_value_type = uint8_t(payload[point_pos + 1]);
			uint8_t point_value_len = uint8_t(payload[point_pos + 2]);
			uint8_t* point_value = payload + (point_pos + 2 + point_value_len);

			ESP_LOGI(TAG, "=== TLV ===");
			//ESP_LOGI(TAG, "Data Type:   0x%02X (%d)", point_type, point_type);
			//ESP_LOGI(TAG, "Value Type:   0x%02X (%d)", point_value_type, point_value_type);
			ESP_LOGI(TAG, "Length: %d bytes", point_value_len);
			//ESP_LOGI(tag, "Value:  %d bytes", value.size());
			point_pos = point_pos + 2 + point_value_len;
		}
		return true;
	}

};
#endif
