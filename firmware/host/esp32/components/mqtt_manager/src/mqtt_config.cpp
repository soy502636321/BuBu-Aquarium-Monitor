/*
 * mqtt_config.cpp
 *
 *  Created on: 2026年7月17日
 *      Author: Hu
 */


#include "mqtt_config.h"
#include "host_info.h"

std::string MqttBrokerConfig::clientId() {
    auto& hostInfo = HostInfo::instance();
    return "BBAM_" + hostInfo.getHostId();  // 例如: BBAM_A1B2C3D4E5F6
}