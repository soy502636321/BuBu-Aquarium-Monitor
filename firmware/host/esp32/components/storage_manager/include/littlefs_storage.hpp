//
// Created by Hu on 2026/7/29.
//

#ifndef APP_TEMPLATE_LITTLEFS_STORAGE_HPP
#define APP_TEMPLATE_LITTLEFS_STORAGE_HPP
#include "base_storage.hpp"
#include "esp_littlefs.h"
#include <string>

class LittleFSStorage : public BaseStorage<LittleFSStorage> {
public:
	LittleFSStorage(const std::string& partition_label = "littlefs",
					const std::string& mount_point = "/littlefs");
	~LittleFSStorage();

	bool init();
	void deinit();
	bool isMounted() const { return m_is_mounted; }

	std::string getFullPath(const std::string& path) const;
	bool ensureParentDirectory(const std::string& full_path);
	void log(const char* format, ...);

	uint64_t getTotalBytes();
	uint64_t getFreeBytes();

	std::string getStorageName() const { return "LittleFS"; }
	std::string getMountPoint() const { return m_mount_point; }

private:
	std::string m_partition_label;
	std::string m_mount_point;
	bool m_is_mounted;
};
#endif // APP_TEMPLATE_LITTLEFS_STORAGE_HPP
