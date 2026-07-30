/*
 * storage_manager.cpp
 *
 *  Created on: 2026年7月23日
 *      Author: Hu
 *  Modified: 切换至 LittleFS
 */
// storage_manager.cpp
#include "storage_manager.hpp"
#include "esp_log.h"

static const char* TAG = "StorageManager";
StorageManager* StorageManager::m_instance = nullptr;

StorageManager& StorageManager::getInstance() {
	if (m_instance == nullptr) {
		m_instance = new StorageManager();
	}
	return *m_instance;
}

bool StorageManager::init() {
	bool all_success = true;

	// 初始化 SD 卡
	if (!m_sd_card.init()) {
		ESP_LOGW(TAG, "SD card init failed: %s", m_sd_card.getLastError().c_str());
		all_success = false;
	}

	// 初始化 LittleFS
	if (!m_littlefs.init()) {
		ESP_LOGW(TAG, "LittleFS init failed: %s", m_littlefs.getLastError().c_str());
		all_success = false;
	}

	// 打印状态
	ESP_LOGI(TAG, "Storage status: SD=%s, LittleFS=%s",
			 m_sd_card.isMounted() ? "OK" : "FAIL",
			 m_littlefs.isMounted() ? "OK" : "FAIL");

	return all_success;
}

void StorageManager::deinit() {
	m_sd_card.deinit();
	m_littlefs.deinit();
	ESP_LOGI(TAG, "All storage deinitialized");
}