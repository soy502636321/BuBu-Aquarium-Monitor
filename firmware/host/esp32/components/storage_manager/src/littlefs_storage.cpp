//
// Created by Hu on 2026/7/29.
//
// littlefs_storage.cpp
#include "littlefs_storage.hpp"
#include <cstdarg>

static const char* TAG = "LittleFSStorage";

LittleFSStorage::LittleFSStorage(const std::string& partition_label,
                                 const std::string& mount_point)
    : m_partition_label(partition_label)
    , m_mount_point(mount_point)
    , m_is_mounted(false) {
}

LittleFSStorage::~LittleFSStorage() {
    deinit();
}

bool LittleFSStorage::init() {
    if (m_is_mounted) return true;

    esp_vfs_littlefs_conf_t conf = {
        .base_path = m_mount_point.c_str(),
        .partition_label = m_partition_label.c_str(),
        .format_if_mount_failed = true,
        .dont_mount = false,
    };

    esp_err_t ret = esp_vfs_littlefs_register(&conf);
    if (ret != ESP_OK) {
        setError("Failed to mount LittleFS: " + std::string(esp_err_to_name(ret)));
        ESP_LOGE(TAG, "%s", getLastError().c_str());
        return false;
    }

    m_is_mounted = true;
    ESP_LOGI(TAG, "LittleFS mounted at %s", m_mount_point.c_str());
    return true;
}

void LittleFSStorage::deinit() {
    if (m_is_mounted) {
        esp_vfs_littlefs_unregister(m_partition_label.c_str());
        m_is_mounted = false;
        ESP_LOGI(TAG, "LittleFS unmounted");
    }
}

std::string LittleFSStorage::getFullPath(const std::string& path) const {
    if (path.find(m_mount_point) == 0) return path;
    std::string full = m_mount_point;
    if (path[0] != '/') full += "/";
    full += path;
    return full;
}

bool LittleFSStorage::ensureParentDirectory(const std::string& full_path) {
    size_t last = full_path.find_last_of('/');
    if (last == std::string::npos || last == m_mount_point.length()) {
        return true;
    }

    std::string dir = full_path.substr(0, last);
    struct stat st;
    if (stat(dir.c_str(), &st) == 0) {
        return S_ISDIR(st.st_mode);
    }

    if (mkdir(dir.c_str(), 0755) != 0) {
        setError("mkdir failed: " + dir);
        return false;
    }
    return true;
}

void LittleFSStorage::log(const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    ESP_LOGD(TAG, "%s", buffer);
}

uint64_t LittleFSStorage::getTotalBytes() {
    if (!m_is_mounted) return 0;
    size_t total = 0, used = 0;
    if (esp_littlefs_info(m_partition_label.c_str(), &total, &used) != ESP_OK) {
        return 0;
    }
    return total;
}

uint64_t LittleFSStorage::getFreeBytes() {
    if (!m_is_mounted) return 0;
    size_t total = 0, used = 0;
    if (esp_littlefs_info(m_partition_label.c_str(), &total, &used) != ESP_OK) {
        return 0;
    }
    return total - used;
}