// host_info.cpp

#include "host_info.h"
#include "esp_log.h"
#include "esp_flash.h"   // ★★★ 如果需要使用 esp_flash_get_size ★★★
#include <cstdio>
#include <cstring>   // ★★★ 添加这个头文件 ★★★

static constexpr const char* TAG = "HostInfo";

// ★★★ 构造函数的实现 ★★★
HostInfo::HostInfo() {
    ESP_LOGI(TAG, "Initializing Host info...");
    initMacAddress();
    initChipInfo();
    ESP_LOGI(TAG, "\t%s\t", getInfo().c_str());
}

void HostInfo::initMacAddress() {
    uint8_t mac[6];
    esp_err_t ret = esp_read_mac(mac, ESP_MAC_WIFI_STA);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read MAC: %s", esp_err_to_name(ret));
        uint8_t defaultMac[6] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55};
        memcpy(mac, defaultMac, 6);
    }

    char buffer[18];
    snprintf(buffer, sizeof(buffer), 
             "%02X%02X%02X%02X%02X%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    macAddress = buffer;

    snprintf(buffer, sizeof(buffer), 
             "%02X:%02X:%02X:%02X:%02X:%02X",
             mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    macAddressWithColon = buffer;

    ESP_LOGI(TAG, "MAC: %s", macAddressWithColon.c_str());
}

void HostInfo::initChipInfo() {
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);

    // 芯片型号
    switch (chip_info.model) {
        case CHIP_ESP32:   m_chipModel = "ESP32"; break;
        case CHIP_ESP32S2: m_chipModel = "ESP32-S2"; break;
        case CHIP_ESP32S3: m_chipModel = "ESP32-S3"; break;
        case CHIP_ESP32C3: m_chipModel = "ESP32-C3"; break;
        case CHIP_ESP32C2: m_chipModel = "ESP32-C2"; break;
        case CHIP_ESP32C6: m_chipModel = "ESP32-C6"; break;
        case CHIP_ESP32H2: m_chipModel = "ESP32-H2"; break;
        default:           m_chipModel = "Unknown"; break;
    }

    m_cpuCores = chip_info.cores;

    // ★★★ 更好的方式：使用 esp_flash_get_size() ★★★
    uint32_t flash_size;
    if (esp_flash_get_size(NULL, &flash_size) == ESP_OK) {
        m_flashSizeMB = flash_size / (1024 * 1024);
    } else {
        m_flashSizeMB = 0;
        ESP_LOGW(TAG, "Failed to get flash size");
    }

    ESP_LOGI(TAG, "Chip: %s, Cores: %d, Flash: %d MB",
             m_chipModel.c_str(), m_cpuCores, m_flashSizeMB);
}

std::string HostInfo::getInfo() const {
    char buffer[1024];
    snprintf(buffer, sizeof(buffer),
             "\n"
             "+------------------------------------------------------+\n"
             "|  Host:      %-40s |\n"
             "|  Model:     %-40s |\n"
             "|  MAC:       %-40s |\n"
             "|  Chip:      %-40s |\n"
             "|  Cores:     %-37d    |\n"
             "|  Flash:     %-37d MB |\n"
             "|  Free Heap: %-34zu bytes |\n"
             "|  Firmware:  v%-39s |\n"
             "+------------------------------------------------------+",
             getProductName().c_str(),
             getHostModel().c_str(),
             getMacAddressWithColon().c_str(),
             getChipModel().c_str(),
             getCpuCores(),
             getFlashSizeMB(),
             getFreeHeapSize(),
             getFirmwareVersion().c_str());
    return std::string(buffer);
}