// DataGateway/inc/DataGateway.hpp
#ifndef DATA_GATEWAY_HPP
#define DATA_GATEWAY_HPP

#include "ChannelManager.hpp"
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

    void transmit(DataContext &ctx) const;  // 发送
	void receive(DataContext &ctx) const; // 接收

private:
	// -------- 单例核心 --------
	DataGateway() = default;   // 空构造
	~DataGateway() = default;  // 空析构

    // 禁止拷贝
    DataGateway(const DataGateway&) = delete;
    DataGateway& operator=(const DataGateway&) = delete;

private:
	// ---
	ChannelManager* m_channel_manager;
	// -------- Pipeline --------
	TxPipeline* m_tx_pipeline;
	RxPipeline* m_rx_pipeline;
	bool m_initialized = false;
};

#endif // DATA_GATEWAY_HPP
