//
// Created by Hu on 2026/7/29.
//

#ifndef APP_TEMPLATE_SDCARD_STORAGE_HPP
#define APP_TEMPLATE_SDCARD_STORAGE_HPP


#include "base_storage.hpp"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include <string>

class SDCardStorage : public BaseStorage<SDCardStorage> {
public:
	SDCardStorage(const std::string& mount_point = "/sdcard");
	~SDCardStorage();

	// ========== 挂载/卸载 ==========
	bool init();
	void deinit();
	bool isMounted() const { return m_is_mounted; }

	// ========== BaseStorage 需要的接口 ==========
	std::string getFullPath(const std::string& path) const;
	bool ensureParentDirectory(const std::string& full_path);
	void log(const char* format, ...);

	// ========== 存储信息 ==========
	uint64_t getTotalBytes();
	uint64_t getFreeBytes();

	// ========== 属性 ==========
	std::string getStorageName() const { return "SDCard"; }
	std::string getMountPoint() const { return m_mount_point; }

private:
	bool mountSDCard();
	void unmountSDCard();

private:
	std::string m_mount_point;
	bool m_is_mounted;
	sdmmc_card_t* m_card;
};

#endif // APP_TEMPLATE_SDCARD_STORAGE_HPP
