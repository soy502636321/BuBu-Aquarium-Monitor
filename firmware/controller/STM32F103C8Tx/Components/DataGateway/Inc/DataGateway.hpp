// DataGateway/inc/DataGateway.hpp
#ifndef DATA_GATEWAY_HPP
#define DATA_GATEWAY_HPP

#include "Pipeline.hpp"

class DataGateway
{

public:
	static DataGateway& getInstance() {
	    static DataGateway instance;  // 线程安全的单例
	    return instance;
	}

	void init();

	// -------- Pipeline 配置 --------
	void setupTxPipeline();
	void setupRxPipeline();

    void transmit(const DataContext& content);  // ✅ 添加声明


private:
	// -------- 单例核心 --------
	DataGateway() = default;   // 空构造
	~DataGateway() = default;  // 空析构

    // 禁止拷贝
    DataGateway(const DataGateway&) = delete;
    DataGateway& operator=(const DataGateway&) = delete;

private:
	// -------- Pipeline --------
	ITxPipeline* m_tx_pipeline;
	IRxPipeline* m_rx_pipeline;
	bool m_initialized = false;
};

#endif // DATA_GATEWAY_HPP
