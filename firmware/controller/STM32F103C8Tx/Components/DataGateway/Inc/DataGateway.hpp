// DataGateway/inc/DataGateway.hpp
#ifndef DATA_GATEWAY_HPP
#define DATA_GATEWAY_HPP

#include "Pipeline.hpp"
#include <vector>
#include <memory>
#include <string>
#include <functional>

namespace DataGateway {

class DataGateway
{

public:
	static DataGateway& getInstance() {
	    static DataGateway instance;  // 线程安全的单例
	    return instance;
	}

	void init();

	void setupPipeline();

    void transmit(const ValveContext& content);  // ✅ 添加声明


private:
	// -------- 单例核心 --------
    DataGateway();  // 私有构造函数
    ~DataGateway();

    // 禁止拷贝
    DataGateway(const DataGateway&) = delete;
    DataGateway& operator=(const DataGateway&) = delete;

	Pipeline pipeline;
	bool initialized = false;
};

} // namespace DataGateway

#endif // DATA_GATEWAY_HPP
