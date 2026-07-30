/*
 * storage_manager.hpp
 *
 *  Created on: 2026年7月23日
 *      Author: Hu
 *  Modified: 切换至 LittleFS
 */

#ifndef COMPONENTS_STORAGE_MANAGER_INCLUDE_STORAGE_MANAGER_HPP_
#define COMPONENTS_STORAGE_MANAGER_INCLUDE_STORAGE_MANAGER_HPP_

#include "sdcard_storage.hpp"
#include "littlefs_storage.hpp"

class StorageManager {
public:
	static StorageManager& getInstance();

	// ========== 初始化所有存储 ==========
	bool initAll();
	void deinitAll();

	// ========== 获取存储实例 ==========
	SDCardStorage& getSDCard() { return m_sd_card; }
	LittleFSStorage& getLittleFS() { return m_littlefs; }

	// ========== 状态查询 ==========
	bool isSDCardReady() const { return m_sd_card.isMounted(); }
	bool isLittleFSReady() const { return m_littlefs.isMounted(); }

	// ========== 便捷别名 ==========
	SDCardStorage& sd() { return m_sd_card; }
	LittleFSStorage& flash() { return m_littlefs; }

	// 禁止拷贝
	StorageManager(const StorageManager&) = delete;
	StorageManager& operator=(const StorageManager&) = delete;

private:
	StorageManager() = default;
	~StorageManager() = default;

private:
	static StorageManager* m_instance;
	SDCardStorage m_sd_card{"/sdcard"};
	LittleFSStorage m_littlefs{"littlefs", "/littlefs"};
};
#endif /* COMPONENTS_STORAGE_MANAGER_INCLUDE_STORAGE_MANAGER_HPP_ */