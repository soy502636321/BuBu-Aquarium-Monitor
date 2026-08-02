// DataGateway/src/DataGateway.cpp
#include "DataGateway.hpp"
#include <cstdio>

namespace DataGateway {

    // -------- 构造/析构 --------
    DataGateway::DataGateway(): pipeline(), initialized(false) {
        printf("[DataGateway] Constructor\n");
        init();
    }

    DataGateway::~DataGateway() {
        printf("[DataGateway] Destructor\n");
    }

    // -------- 初始化 --------
    void DataGateway::init() {
        if (initialized) {
            printf("[DataGateway] Already initialized\n");
            return;
        }

        printf("[DataGateway] Initializing...\n");
        setupPipeline();
        initialized = true;
        printf("[DataGateway] Initialized, pipeline size: %zu\n", pipeline.size());
    }

    // -------- 设置 Pipeline --------
    void DataGateway::setupPipeline() {
        printf("[DataGateway] Setting up pipeline...\n");
        pipeline.clear();

        // TODO: 添加阀门
        // pipeline.addValve(new LogValve());
        // pipeline.addValve(new ValidateValve());

        printf("[DataGateway] Pipeline setup complete\n");
    }

    // -------- 传输 --------
    void DataGateway::transmit(const ValveContext& content) {
        // if (!initialized) {
        //     printf("[DataGateway] Not initialized!\n");
        //     return;
        // }

        printf("[DataGateway] Transmitting...\n");

        // if (content.packet != nullptr) {
        //     printf("  Packet: seq=%u, data_size=%zu\n",
        //            content.packet->getSequence(),
        //            content.packet->getData().size());
        // }

        // 执行 pipeline
        // ValveContext& ctx = const_cast<ValveContext&>(content);
        // bool result = pipeline.execute(ctx);
        //
        // printf("[DataGateway] Transmit %s\n", result ? "SUCCESS" : "FAIL");
    }

} // namespace DataGateway