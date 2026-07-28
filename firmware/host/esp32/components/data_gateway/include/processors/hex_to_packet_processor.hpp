#ifndef HEX_TO_PACKET_PROCESSOR
#define HEX_TO_PACKET_PROCESSOR
/**
 * 负责把原始的raw解析成具体的数据给下一步
 */
#include "data_processor.hpp"
#include "device.hpp"

#include "esp_log.h"

#define TAG "BuBu-Aquarium-Monitor[hex_to_packet_processor]"

class HexToPacketProcessor : public IDataProcessor
{

public:

	bool execute(DataContext& ctx) override
	{
		ESP_LOGI(TAG, "HexToPacketProcessor");
		return true;
	}

};
#endif
