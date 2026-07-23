/*
 * fs_manager.cpp
 *
 *  Created on: 2026年7月23日
 *      Author: Hu
 *  Modified: 切换至 LittleFS
 */
#include "fs_manager.hpp"
#include "esp_littlefs.h"
#include "esp_log.h"
#include <cstring>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>

static const char* TAG = "BuBu-Aquarium-Monitor[fs_manager]";

// ==================== 静态成员初始化 ====================

FSManager* FSManager::m_instance = nullptr;

// ==================== 单例管理 ====================

FSManager& FSManager::getInstance() {
    if (m_instance == nullptr) {
        m_instance = new FSManager();
    }
    return *m_instance;
}

// ==================== 构造函数/析构函数 ====================

FSManager::FSManager() 
    : m_mount_point(LITTLEFS_BASE_PATH)
    , m_last_error("")
    , m_is_mounted(false) {
    ESP_LOGI(TAG, "FSManager created");
}

FSManager::~FSManager() {
    deinit();
    m_instance = nullptr;
}

// ==================== 初始化/反初始化 ====================

bool FSManager::init(bool format_if_mount_failed) {
    if (m_is_mounted) {
        ESP_LOGW(TAG, "LittleFS already mounted");
        return true;
    }
    
    ESP_LOGI(TAG, "Initializing LittleFS on %s", m_mount_point.c_str());
    
    esp_vfs_littlefs_conf_t conf = {
        .base_path = m_mount_point.c_str(),
        .partition_label = "littlefs",      // 必须与分区表中的名称一致
        .format_if_mount_failed = format_if_mount_failed,
        .dont_mount = false,
    };
    
    esp_err_t ret = esp_vfs_littlefs_register(&conf);
    
    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            setError("Failed to mount or format filesystem");
        } else if (ret == ESP_ERR_NOT_FOUND) {
            setError("Failed to find LittleFS partition");
        } else {
            setError("Failed to init LittleFS: " + std::string(esp_err_to_name(ret)));
        }
        ESP_LOGE(TAG, "%s", m_last_error.c_str());
        return false;
    }
    
    m_is_mounted = true;
    
    // 打印分区信息
    size_t total = 0, used = 0;
    if (esp_littlefs_info("littlefs", &total, &used) == ESP_OK) {
        ESP_LOGI(TAG, "LittleFS mounted. Total: %zu KB, Used: %zu KB, Free: %zu KB", 
                 total / 1024, used / 1024, (total - used) / 1024);
    }
    
    setError("");
    return true;
}

void FSManager::deinit() {
    if (m_is_mounted) {
        esp_vfs_littlefs_unregister("littlefs");
        m_is_mounted = false;
        ESP_LOGI(TAG, "LittleFS unmounted");
    }
}

// ==================== 内部辅助函数 ====================

std::string FSManager::getFullPath(const std::string& relative_path) const {
    if (relative_path.empty()) {
        return m_mount_point;
    }
    
    // 如果路径已经以挂载点开头，直接返回
    if (relative_path.find(m_mount_point) == 0) {
        return relative_path;
    }
    
    // 拼接完整路径
    std::string full_path = m_mount_point;
    if (relative_path[0] != '/') {
        full_path += "/";
    }
    full_path += relative_path;
    
    return full_path;
}

void FSManager::setError(const std::string& error) {
    m_last_error = error;
}

bool FSManager::ensureParentDirectoryExists(const std::string& path) {
    std::string full_path = getFullPath(path);
    
    // 找到最后一个 '/'，提取目录部分
    size_t last_slash = full_path.find_last_of('/');
    if (last_slash == std::string::npos || last_slash <= m_mount_point.length()) {
        return true; // 没有父目录或父目录就是挂载点
    }
    
    std::string dir_path = full_path.substr(0, last_slash);
    
    // 检查目录是否存在
    struct stat st;
    if (stat(dir_path.c_str(), &st) == 0) {
        return S_ISDIR(st.st_mode);
    }
    
    // 递归创建父目录
    if (mkdir(dir_path.c_str(), 0755) != 0) {
        setError("Failed to create directory: " + dir_path);
        return false;
    }
    
    ESP_LOGD(TAG, "Created directory: %s", dir_path.c_str());
    return true;
}

// ==================== 文件写入 ====================

bool FSManager::writeFile(const std::string& path, const std::string& data) {
    if (!m_is_mounted) {
        setError("LittleFS not mounted");
        return false;
    }
    
    // 确保父目录存在
    if (!ensureParentDirectoryExists(path)) {
        return false;
    }
    
    std::string full_path = getFullPath(path);
    
    FILE* file = fopen(full_path.c_str(), "w");
    if (file == nullptr) {
        setError("Failed to open file for writing: " + full_path);
        ESP_LOGE(TAG, "%s", m_last_error.c_str());
        return false;
    }
    
    size_t written = fwrite(data.c_str(), 1, data.length(), file);
    fclose(file);
    
    if (written != data.length()) {
        setError("Failed to write complete data");
        ESP_LOGE(TAG, "%s", m_last_error.c_str());
        return false;
    }
    
    ESP_LOGI(TAG, "File written: %s (%zu bytes)", full_path.c_str(), written);
    return true;
}

// ==================== 文件读取 ====================

bool FSManager::readFile(const std::string& path, std::string& out_data) {
    if (!m_is_mounted) {
        setError("LittleFS not mounted");
        return false;
    }
    
    std::string full_path = getFullPath(path);
    
    FILE* file = fopen(full_path.c_str(), "r");
    if (file == nullptr) {
        setError("Failed to open file for reading: " + full_path);
        ESP_LOGE(TAG, "%s", m_last_error.c_str());
        return false;
    }
    
    // 获取文件大小
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (size <= 0) {
        fclose(file);
        out_data.clear();
        return true;
    }
    
    // 读取数据
    out_data.resize(size);
    size_t read_bytes = fread(&out_data[0], 1, size, file);
    fclose(file);
    
    if (read_bytes != (size_t)size) {
        setError("Failed to read complete file");
        ESP_LOGE(TAG, "%s", m_last_error.c_str());
        return false;
    }
    
    ESP_LOGI(TAG, "File read: %s (%zu bytes)", full_path.c_str(), read_bytes);
    return true;
}

// ==================== 文件追加 ====================

bool FSManager::appendFile(const std::string& path, const std::string& data) {
    if (!m_is_mounted) {
        setError("LittleFS not mounted");
        return false;
    }
    
    // 确保父目录存在
    if (!ensureParentDirectoryExists(path)) {
        return false;
    }
    
    std::string full_path = getFullPath(path);
    
    FILE* file = fopen(full_path.c_str(), "a");
    if (file == nullptr) {
        setError("Failed to open file for append: " + full_path);
        ESP_LOGE(TAG, "%s", m_last_error.c_str());
        return false;
    }
    
    size_t written = fwrite(data.c_str(), 1, data.length(), file);
    fclose(file);
    
    if (written != data.length()) {
        setError("Failed to append complete data");
        ESP_LOGE(TAG, "%s", m_last_error.c_str());
        return false;
    }
    
    ESP_LOGI(TAG, "File appended: %s (%zu bytes)", full_path.c_str(), written);
    return true;
}

// ==================== 文件信息 ====================

bool FSManager::fileExists(const std::string& path) {
    if (!m_is_mounted) {
        setError("LittleFS not mounted");
        return false;
    }
    
    std::string full_path = getFullPath(path);
    struct stat st;
    return (stat(full_path.c_str(), &st) == 0);
}

bool FSManager::deleteFile(const std::string& path) {
    if (!m_is_mounted) {
        setError("LittleFS not mounted");
        return false;
    }
    
    std::string full_path = getFullPath(path);
    if (unlink(full_path.c_str()) != 0) {
        setError("Failed to delete file: " + full_path);
        ESP_LOGE(TAG, "%s", m_last_error.c_str());
        return false;
    }
    
    ESP_LOGI(TAG, "File deleted: %s", full_path.c_str());
    return true;
}

bool FSManager::getFileSize(const std::string& path, size_t& out_size) {
    if (!m_is_mounted) {
        setError("LittleFS not mounted");
        return false;
    }
    
    std::string full_path = getFullPath(path);
    struct stat st;
    if (stat(full_path.c_str(), &st) != 0) {
        setError("File not found: " + full_path);
        return false;
    }
    
    out_size = st.st_size;
    return true;
}

// ==================== 目录操作 ====================

bool FSManager::createDirectory(const std::string& path) {
    if (!m_is_mounted) {
        setError("LittleFS not mounted");
        return false;
    }
    
    std::string full_path = getFullPath(path);
    
    if (directoryExists(path)) {
        return true;
    }
    
    if (mkdir(full_path.c_str(), 0755) != 0) {
        setError("Failed to create directory: " + full_path);
        ESP_LOGE(TAG, "%s", m_last_error.c_str());
        return false;
    }
    
    ESP_LOGI(TAG, "Directory created: %s", full_path.c_str());
    return true;
}

bool FSManager::directoryExists(const std::string& path) {
    if (!m_is_mounted) {
        setError("LittleFS not mounted");
        return false;
    }
    
    std::string full_path = getFullPath(path);
    struct stat st;
    
    if (stat(full_path.c_str(), &st) != 0) {
        // 路径不存在，这不是错误，只是返回 false
        setError("");  // 清空错误信息
        return false;
    }
    
    if (S_ISDIR(st.st_mode)) {
        setError("");  // 清空错误信息
        return true;
    } else {
        setError("Path exists but is not a directory: " + full_path);
        return false;
    }
}

std::vector<std::string> FSManager::listDirectory(const std::string& path) {
    std::vector<std::string> items;
    
    if (!m_is_mounted) {
        setError("LittleFS not mounted");
        return items;
    }
    
    std::string full_path = getFullPath(path);
    
    DIR* dir = opendir(full_path.c_str());
    if (dir == nullptr) {
        setError("Failed to open directory: " + full_path);
        ESP_LOGE(TAG, "%s", m_last_error.c_str());
        return items;
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        // 跳过 . 和 ..
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        items.push_back(entry->d_name);
        ESP_LOGD(TAG, "Found: %s", entry->d_name);
    }
    
    closedir(dir);
    ESP_LOGI(TAG, "Listed %zu items in %s", items.size(), full_path.c_str());
    return items;
}

bool FSManager::deleteDirectory(const std::string& path) {
    if (!m_is_mounted) {
        setError("LittleFS not mounted");
        return false;
    }
    
    std::string full_path = getFullPath(path);
    
    // 递归删除所有内容
    DIR* dir = opendir(full_path.c_str());
    if (dir == nullptr) {
        setError("Failed to open directory: " + full_path);
        return false;
    }
    
    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }
        
        std::string item_path = full_path + "/" + entry->d_name;
        struct stat st;
        if (stat(item_path.c_str(), &st) == 0) {
            if (S_ISDIR(st.st_mode)) {
                // 递归删除子目录
                deleteDirectory(item_path);
            } else {
                // 删除文件
                unlink(item_path.c_str());
            }
        }
    }
    closedir(dir);
    
    // 删除空目录
    if (rmdir(full_path.c_str()) != 0) {
        setError("Failed to delete directory: " + full_path);
        return false;
    }
    
    ESP_LOGI(TAG, "Directory deleted: %s", full_path.c_str());
    return true;
}

// ==================== 存储信息 ====================

size_t FSManager::getTotalBytes() {
    if (!m_is_mounted) {
        setError("LittleFS not mounted");
        return 0;
    }
    
    size_t total = 0, used = 0;
    if (esp_littlefs_info("littlefs", &total, &used) != ESP_OK) {
        setError("Failed to get LittleFS info");
        return 0;
    }
    
    return total;
}

size_t FSManager::getFreeBytes() {
    if (!m_is_mounted) {
        setError("LittleFS not mounted");
        return 0;
    }
    
    size_t total = 0, used = 0;
    if (esp_littlefs_info("littlefs", &total, &used) != ESP_OK) {
        setError("Failed to get LittleFS info");
        return 0;
    }
    
    return total - used;
}