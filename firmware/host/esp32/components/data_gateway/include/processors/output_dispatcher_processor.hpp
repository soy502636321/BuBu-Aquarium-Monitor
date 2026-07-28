//
// Created by Hu on 2026-07-28.
//

#ifndef ESP32_OUTPUT_PROCESSOR_HPP
#define ESP32_OUTPUT_PROCESSOR_HPP

#include "data_processor.hpp"
#include "output/data_output.hpp"
#include "output/esp_log_output.hpp"

class OutputDispatcherProcessor : public IDataProcessor
{

public:
	OutputDispatcherProcessor()
	{
		registerOutput(new EspLogOutput()); // 注册分发到日志 为了调试
	}

	void registerOutput(
		IDataOutput* output)
	{
		outputs.push_back(output);
	}

	bool execute(DataContext& context)
	{
		for(auto output : outputs)
		{
			output->handle(context);
		}
		return true;
	}

private:

	std::vector<IDataOutput*> outputs; // 输出集合

};

#endif // ESP32_OUTPUT_PROCESSOR_HPP
