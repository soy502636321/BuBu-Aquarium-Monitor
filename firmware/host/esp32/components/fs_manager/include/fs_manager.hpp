/*
 * fs_manager.hpp
 *
 *  Created on: 2026年7月23日
 *      Author: Hu
 *  Modified: 切换至 LittleFS
 */

#ifndef COMPONENTS_FS_MANAGER_INCLUDE_FS_MANAGER_HPP_
#define COMPONENTS_FS_MANAGER_INCLUDE_FS_MANAGER_HPP_

#include <string>
#include <vector>
#include "esp_err.h"

// 挂载点路径（可以保持 /spiffs 以兼容旧代码，或改成 /littlefs）
#define LITTLEFS_BASE_PATH "/littlefs"

class FSManager {
public:
    // 获取单例实例
    static FSManager& getInstance();
    
    // 禁止拷贝和赋值
    FSManager(const FSManager&) = delete;
    FSManager& operator=(const FSManager&) = delete;
    
    // ========== 基础功能 ==========
    
    /**
     * @brief 初始化 LittleFS
     * @param format_if_mount_failed 挂载失败时是否格式化
     * @return true=成功, false=失败
     */
    bool init(bool format_if_mount_failed = true);
    
    /**
     * @brief 卸载 LittleFS
     */
    void deinit();
    
    /**
     * @brief 检查是否已挂载
     */
    bool isMounted() const { return m_is_mounted; }
    
    // ========== 文件操作 ==========
    
    /**
     * @brief 写入文件（覆盖）
     * @param path 文件路径（相对路径，如 "/test.txt"）
     * @param data 要写入的数据
     * @return true=成功, false=失败
     */
    bool writeFile(const std::string& path, const std::string& data);
    
    /**
     * @brief 读取文件
     * @param path 文件路径
     * @param out_data 输出的数据
     * @return true=成功, false=失败
     */
    bool readFile(const std::string& path, std::string& out_data);
    
    /**
     * @brief 追加写入文件
     * @param path 文件路径
     * @param data 要追加的数据
     * @return true=成功, false=失败
     */
    bool appendFile(const std::string& path, const std::string& data);
    
    /**
     * @brief 检查文件是否存在
     */
    bool fileExists(const std::string& path);
    
    /**
     * @brief 删除文件
     */
    bool deleteFile(const std::string& path);
    
    /**
     * @brief 获取文件大小
     */
    bool getFileSize(const std::string& path, size_t& out_size);
    
    // ========== 目录操作 ==========
    
    /**
     * @brief 创建目录（LittleFS 原生支持目录）
     */
    bool createDirectory(const std::string& path);
    
    /**
     * @brief 列出目录内容
     * @param path 目录路径
     * @return 文件/目录名列表
     */
    std::vector<std::string> listDirectory(const std::string& path);
    
    /**
     * @brief 判断目录是否存在
     */
    bool directoryExists(const std::string& path);  
    
    /**
     * @brief 删除目录（递归删除所有内容）
     */
    bool deleteDirectory(const std::string& path);
    
    // ========== 工具方法 ==========
    
    /**
     * @brief 获取最后错误信息
     */
    std::string getLastError() const { return m_last_error; }
    
    /**
     * @brief 获取挂载点
     */
    std::string getMountPoint() const { return m_mount_point; }
    
    /**
     * @brief 获取总容量（字节）
     */
    size_t getTotalBytes();
    
    /**
     * @brief 获取剩余容量（字节）
     */
    size_t getFreeBytes();

private:
    // 私有构造函数
    FSManager();
    ~FSManager();
    
    // 内部辅助函数
    std::string getFullPath(const std::string& relative_path) const;
    void setError(const std::string& error);
    bool ensureParentDirectoryExists(const std::string& path);

private:
    static FSManager* m_instance;
    
    std::string m_mount_point;      // 挂载点，如 "/littlefs"
    std::string m_last_error;       // 最后错误信息
    bool m_is_mounted;              // 挂载状态
};

#endif /* COMPONENTS_FS_MANAGER_INCLUDE_FS_MANAGER_HPP_ */