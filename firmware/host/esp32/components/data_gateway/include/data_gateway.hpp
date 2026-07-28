/*
 * 数据网关主接口
 *
 *  Created on: 2026年7月23日
 *      Author: Hu
 */

#ifndef COMPONENTS_DATA_GATEWAY_INCLUDE_DATA_GATEWAY_HPP_
#define COMPONENTS_DATA_GATEWAY_INCLUDE_DATA_GATEWAY_HPP_

#include <string>
#include "device.hpp"
#include "data_receiver.hpp"
#include "esp_log.h"
#include "data_pipeline.hpp"

// 网关配置
struct DataGatewayConfig {
    bool auto_start = true;              // 是否自动启动
    uint32_t task_stack_size = 4096;     // 任务栈大小
    uint8_t task_priority = 5;           // 任务优先级
    std::string default_protocol = "ttl"; // 默认协议
};

// 数据网关 - 主接口类
class DataGateway
{

public:
	static DataGateway &instance();


	void init();

	void setupPipeline();

	void receive(const uint8_t* data, size_t len, DataSource source);

private:
	// -------- 单例核心 --------
	DataGateway() {
		// ✅ 构造函数中调用 init()
		init();
	}
	~DataGateway() = default;
	DataGateway(const DataGateway&) = delete;
	DataGateway& operator=(const DataGateway&) = delete;

	DataPipeline pipeline;
	bool initialized = false;
};
#endif /* COMPONENTS_DATA_GATEWAY_INCLUDE_DATA_GATEWAY_HPP_ */
