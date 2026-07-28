//
// Created by Hu on 2026-07-28.
//

#ifndef ESP32_DATA_PIPELINE_HPP
#define ESP32_DATA_PIPELINE_HPP

#include <vector>
#include "data_processor.hpp"
#include "data_context.hpp"

class DataPipeline {
private:
	std::vector<IDataProcessor*> processors;

public:
	// 链式添加处理器
	DataPipeline& addProcessor(IDataProcessor* processor) {
		processors.push_back(processor);
		return *this;
	}

	// 执行管道
	bool process(DataContext& context) {
		for (auto* p : processors) {
			if (!p->execute(context)) {
				// 处理器返回 false，管道停止
				ESP_LOGW("PIPELINE", "Processor failed, stopping pipeline");
				return false;
			}
		}
		return true;
	}

	// 获取处理器数量
	size_t size() const {
		return processors.size();
	}

	// 清空所有处理器
	void clear() {
		processors.clear();
	}
};

#endif // ESP32_DATA_PIPELINE_HPP
