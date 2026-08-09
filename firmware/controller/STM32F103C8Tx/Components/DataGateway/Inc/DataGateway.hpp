// DataGateway/inc/DataGateway.hpp
#ifndef DATA_GATEWAY_HPP
#define DATA_GATEWAY_HPP

#include "FreeRTOS.h"
#include "queue.h"

#include "ActionExecutor.hpp"
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

	TxPipeline& getTxPipeline() {
		return m_tx_pipeline;
	}

	void transmit(DataContext &ctx);

	void processRxData(DataContext &ctx);
	void onReceiveFromISR(DataContext* ctx);

	void setupRxPipeline();

	RxPipeline& getRxPipeline() {
		return m_rx_pipeline;
	}

    void transmit(DataContext &ctx) const;  // 发送
	void onReceiveData(DataContext &ctx) const; // 接收

private:
	// 内部任务入口
	static void rxTaskEntry(void* pvParameters);
	static void txTaskEntry(void* pvParameters);

	void rxProcessLoop();
	void txProcessLoop();

	// 核心处理函数（纯逻辑）
	bool processRxData(DataContext* ctx);
	bool processTxData(DataContext* ctx);

private:
	// -------- 单例核心 --------
	DataGateway() = default;   // 空构造
	~DataGateway() = default;  // 空析构
    // 禁止拷贝
    DataGateway(const DataGateway&) = delete;
    DataGateway& operator=(const DataGateway&) = delete;

	// ---
	ChannelManager* m_channel_manager;
	// -------- Pipeline --------
	TxPipeline m_tx_pipeline {};
	RxPipeline m_rx_pipeline {};

	QueueHandle_t m_rx_pipeline_queue = nullptr;	//数据读取处理队列
	QueueHandle_t m_tx_pipeline_queue = nullptr;		//数据发送处理队列

	// ========== 任务句柄 ==========
	TaskHandle_t m_rx_task_handle = nullptr;
	TaskHandle_t m_tx_task_handle = nullptr;

	bool m_initialized = false;
};

#endif // DATA_GATEWAY_HPP
