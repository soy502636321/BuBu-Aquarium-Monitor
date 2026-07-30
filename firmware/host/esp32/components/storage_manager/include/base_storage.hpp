//
// Created by Hu on 2026/7/29.
//

#ifndef APP_TEMPLATE_BASE_STORAGE_HPP
#define APP_TEMPLATE_BASE_STORAGE_HPP

#include <string>
#include <vector>
#include <cstdio>
#include <cstring>
#include <sys/stat.h>
#include <dirent.h>
#include <unistd.h>

template<typename Impl>
class BaseStorage {
public:
    // ========== 文件操作 ==========

    bool writeFile(const std::string& path, const std::string& data) {
        if (!impl().isMounted()) {
            impl().setError("Storage not mounted");
            return false;
        }

        std::string full_path = impl().getFullPath(path);
        if (!impl().ensureParentDirectory(full_path)) {
            return false;
        }

        FILE* f = fopen(full_path.c_str(), "w");
        if (!f) {
            impl().setError("fopen failed: " + full_path);
            return false;
        }

        size_t written = fwrite(data.c_str(), 1, data.size(), f);
        fclose(f);

        if (written != data.size()) {
            impl().setError("Write incomplete");
            return false;
        }

        impl().log("Written: %s (%zu bytes)", full_path.c_str(), written);
        return true;
    }

    bool writeFile(const std::string& path, const std::vector<uint8_t>& data) {
        if (!impl().isMounted()) {
            impl().setError("Storage not mounted");
            return false;
        }

        std::string full_path = impl().getFullPath(path);
        if (!impl().ensureParentDirectory(full_path)) {
            return false;
        }

        FILE* f = fopen(full_path.c_str(), "wb");
        if (!f) {
            impl().setError("fopen failed: " + full_path);
            return false;
        }

        size_t written = fwrite(data.data(), 1, data.size(), f);
        fclose(f);

        if (written != data.size()) {
            impl().setError("Write incomplete");
            return false;
        }

        impl().log("Binary written: %s (%zu bytes)", full_path.c_str(), written);
        return true;
    }

    bool appendFile(const std::string& path, const std::string& data) {
        if (!impl().isMounted()) {
            impl().setError("Storage not mounted");
            return false;
        }

        std::string full_path = impl().getFullPath(path);
        if (!impl().ensureParentDirectory(full_path)) {
            return false;
        }

        FILE* f = fopen(full_path.c_str(), "a");
        if (!f) {
            impl().setError("fopen append failed: " + full_path);
            return false;
        }

        size_t written = fwrite(data.c_str(), 1, data.size(), f);
        fclose(f);

        if (written != data.size()) {
            impl().setError("Append incomplete");
            return false;
        }

        impl().log("Appended: %s (%zu bytes)", full_path.c_str(), written);
        return true;
    }

    bool readFile(const std::string& path, std::string& out_data) {
        if (!impl().isMounted()) {
            impl().setError("Storage not mounted");
            return false;
        }

        std::string full_path = impl().getFullPath(path);
        FILE* f = fopen(full_path.c_str(), "r");
        if (!f) {
            impl().setError("fopen read failed: " + full_path);
            return false;
        }

        fseek(f, 0, SEEK_END);
        long size = ftell(f);
        fseek(f, 0, SEEK_SET);

        if (size <= 0) {
            fclose(f);
            out_data.clear();
            return true;
        }

        out_data.resize(size);
        size_t read_bytes = fread(&out_data[0], 1, size, f);
        fclose(f);

        if (read_bytes != (size_t)size) {
            impl().setError("Read incomplete");
            return false;
        }

        impl().log("Read: %s (%zu bytes)", full_path.c_str(), read_bytes);
        return true;
    }

    bool readFileBinary(const std::string& path, std::vector<uint8_t>& out_data) {
        if (!impl().isMounted()) {
            impl().setError("Storage not mounted");
            return false;
        }

        std::string full_path = impl().getFullPath(path);
        FILE* f = fopen(full_path.c_str(), "rb");
        if (!f) {
            impl().setError("fopen read failed: " + full_path);
            return false;
        }

        fseek(f, 0, SEEK_END);
        long size = ftell(f);
        fseek(f, 0, SEEK_SET);

        if (size <= 0) {
            fclose(f);
            out_data.clear();
            return true;
        }

        out_data.resize(size);
        size_t read_bytes = fread(out_data.data(), 1, size, f);
        fclose(f);

        if (read_bytes != (size_t)size) {
            impl().setError("Read incomplete");
            return false;
        }

        impl().log("Binary read: %s (%zu bytes)", full_path.c_str(), read_bytes);
        return true;
    }

    bool fileExists(const std::string& path) {
        if (!impl().isMounted()) return false;
        std::string full_path = impl().getFullPath(path);
        struct stat st;
        return stat(full_path.c_str(), &st) == 0;
    }

    bool deleteFile(const std::string& path) {
        if (!impl().isMounted()) {
            impl().setError("Storage not mounted");
            return false;
        }

        std::string full_path = impl().getFullPath(path);
        if (unlink(full_path.c_str()) != 0) {
            impl().setError("unlink failed: " + full_path);
            return false;
        }

        impl().log("Deleted: %s", full_path.c_str());
        return true;
    }

    uint64_t getFileSize(const std::string& path) {
        if (!impl().isMounted()) return 0;
        std::string full_path = impl().getFullPath(path);
        struct stat st;
        if (stat(full_path.c_str(), &st) != 0) return 0;
        return st.st_size;
    }

    // ========== 目录操作 ==========

    bool createDirectory(const std::string& path) {
        if (!impl().isMounted()) {
            impl().setError("Storage not mounted");
            return false;
        }

        std::string full_path = impl().getFullPath(path);

        struct stat st;
        if (stat(full_path.c_str(), &st) == 0) {
            return S_ISDIR(st.st_mode);
        }

        // 递归创建
        size_t pos = 0;
        while ((pos = full_path.find('/', pos + 1)) != std::string::npos) {
            std::string sub = full_path.substr(0, pos);
            if (stat(sub.c_str(), &st) != 0) {
                if (mkdir(sub.c_str(), 0755) != 0) {
                    impl().setError("mkdir failed: " + sub);
                    return false;
                }
            }
        }

        if (mkdir(full_path.c_str(), 0755) != 0) {
            impl().setError("mkdir failed: " + full_path);
            return false;
        }

        impl().log("Directory created: %s", full_path.c_str());
        return true;
    }

    bool directoryExists(const std::string& path) {
        if (!impl().isMounted()) return false;
        std::string full_path = impl().getFullPath(path);
        struct stat st;
        if (stat(full_path.c_str(), &st) != 0) return false;
        return S_ISDIR(st.st_mode);
    }

    std::vector<std::string> listDirectory(const std::string& path) {
        std::vector<std::string> items;
        if (!impl().isMounted()) {
            impl().setError("Storage not mounted");
            return items;
        }

        std::string full_path = impl().getFullPath(path);
        DIR* dir = opendir(full_path.c_str());
        if (!dir) {
            impl().setError("opendir failed: " + full_path);
            return items;
        }

        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (strcmp(entry->d_name, ".") != 0 &&
                strcmp(entry->d_name, "..") != 0) {
                items.push_back(entry->d_name);
            }
        }
        closedir(dir);
        return items;
    }

    bool deleteDirectory(const std::string& path) {
        if (!impl().isMounted()) {
            impl().setError("Storage not mounted");
            return false;
        }

        std::string full_path = impl().getFullPath(path);

        DIR* dir = opendir(full_path.c_str());
        if (!dir) {
            impl().setError("opendir failed: " + full_path);
            return false;
        }

        struct dirent* entry;
        while ((entry = readdir(dir)) != nullptr) {
            if (strcmp(entry->d_name, ".") == 0 ||
                strcmp(entry->d_name, "..") == 0) {
                continue;
            }

            std::string item = full_path + "/" + entry->d_name;
            struct stat st;
            if (stat(item.c_str(), &st) == 0) {
                if (S_ISDIR(st.st_mode)) {
                    deleteDirectory(item);
                } else {
                    unlink(item.c_str());
                }
            }
        }
        closedir(dir);

        if (rmdir(full_path.c_str()) != 0) {
            impl().setError("rmdir failed: " + full_path);
            return false;
        }

        impl().log("Directory deleted: %s", full_path.c_str());
        return true;
    }

    // ========== CSV 操作 ==========

    bool writeCSVRow(const std::string& path, const std::vector<std::string>& columns) {
        if (columns.empty()) {
            impl().setError("Empty CSV row");
            return false;
        }

        std::string line;
        for (size_t i = 0; i < columns.size(); ++i) {
            if (i > 0) line += ",";
            line += columns[i];
        }
        line += "\n";
        return writeFile(path, line);
    }

    bool appendCSVRow(const std::string& path, const std::vector<std::string>& columns) {
        if (columns.empty()) {
            impl().setError("Empty CSV row");
            return false;
        }

        std::string line;
        for (size_t i = 0; i < columns.size(); ++i) {
            if (i > 0) line += ",";
            line += columns[i];
        }
        line += "\n";
        return appendFile(path, line);
    }

    std::vector<std::vector<std::string>> readCSV(const std::string& path) {
        std::vector<std::vector<std::string>> result;
        std::string content;
        if (!readFile(path, content)) {
            return result;
        }

        std::stringstream ss(content);
        std::string line;
        while (std::getline(ss, line)) {
            if (line.empty()) continue;
            std::vector<std::string> row;
            std::stringstream row_ss(line);
            std::string field;
            while (std::getline(row_ss, field, ',')) {
                row.push_back(field);
            }
            result.push_back(row);
        }
        return result;
    }

    // ========== 错误信息 ==========
    std::string getLastError() const { return m_last_error; }
    void setError(const std::string& error) { m_last_error = error; }

protected:
    // 子类需要实现的接口
    bool isMounted() const { return impl().isMounted(); }

private:
    Impl& impl() { return static_cast<Impl&>(*this); }
    const Impl& impl() const { return static_cast<const Impl&>(*this); }

private:
    std::string m_last_error;
};

#endif // APP_TEMPLATE_BASE_STORAGE_HPP
