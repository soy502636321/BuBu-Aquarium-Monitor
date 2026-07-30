//
// Created by Hu on 2026/7/29.
//
// sdcard_storage.cpp
#include "sdcard_storage.hpp"
#include "driver/gpio.h"
#include <cstdarg>

static const char* TAG = "SDCardStorage";

SDCardStorage::SDCardStorage(const std::string& mount_point)
    : m_mount_point(mount_point)
    , m_is_mounted(false)
    , m_card(nullptr) {
}

SDCardStorage::~SDCardStorage() {
    deinit();
}

bool SDCardStorage::init() {
    if (m_is_mounted) return true;
    return mountSDCard();
}

void SDCardStorage::deinit() {
    if (m_is_mounted) {
        unmountSDCard();
        m_is_mounted = false;
    }
}

bool SDCardStorage::mountSDCard() {
    sdmmc_host_t host = SDMMC_HOST_DEFAULT();
    host.max_freq_khz = 20000;

    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();
    slot_config.clk = GPIO_NUM_5;
    slot_config.cmd = GPIO_NUM_4;
    slot_config.d0 = GPIO_NUM_6;
    slot_config.d1 = GPIO_NUM_7;
    slot_config.d2 = GPIO_NUM_2;
    slot_config.d3 = GPIO_NUM_3;
    slot_config.width = 4;

    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = false,
        .max_files = 5,
        .allocation_unit_size = 16 * 1024,
    };

    esp_err_t ret = esp_vfs_fat_sdmmc_mount(
        m_mount_point.c_str(),
        &host,
        &slot_config,
        &mount_config,
        &m_card
    );

    if (ret != ESP_OK) {
        setError("Failed to mount SD card: " + std::string(esp_err_to_name(ret)));
        ESP_LOGE(TAG, "%s", getLastError().c_str());
        return false;
    }

    m_is_mounted = true;
    ESP_LOGI(TAG, "SD card mounted at %s", m_mount_point.c_str());
    return true;
}

void SDCardStorage::unmountSDCard() {
    if (m_card) {
        esp_vfs_fat_sdcard_unmount(m_mount_point.c_str(), m_card);
        m_card = nullptr;
    }
}

std::string SDCardStorage::getFullPath(const std::string& path) const {
    if (path.find(m_mount_point) == 0) return path;
    std::string full = m_mount_point;
    if (path[0] != '/') full += "/";
    full += path;
    return full;
}

bool SDCardStorage::ensureParentDirectory(const std::string& full_path) {
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

void SDCardStorage::log(const char* format, ...) {
    va_list args;
    va_start(args, format);
    char buffer[256];
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);
    ESP_LOGD(TAG, "%s", buffer);
}

uint64_t SDCardStorage::getTotalBytes() {
    if (!m_is_mounted || !m_card) return 0;
    return (uint64_t)m_card->csd.capacity * m_card->csd.sector_size;
}

uint64_t SDCardStorage::getFreeBytes() {
    if (!m_is_mounted) return 0;
    FATFS* fs;
    DWORD fre_clust;
    FRESULT res = f_getfree(m_mount_point.c_str(), &fre_clust, &fs);
    if (res != FR_OK) return 0;
    return (uint64_t)fre_clust * fs->csize * 512;
}