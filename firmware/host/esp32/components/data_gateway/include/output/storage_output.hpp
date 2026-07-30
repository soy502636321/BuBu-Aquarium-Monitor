//
// Created by Hu on 2026-07-28.
// 本地存储
//

#ifndef ESP32_STORAGE_OUTPUT_HPP
#define ESP32_STORAGE_OUTPUT_HPP

#define TAG "BuBu-Aquarium-Monitor[storage_output]"

#include "data_output.hpp"
#include "storage_manager.hpp"
#include "esp_log.h"

class StorageOutput : public IDataOutput
{

public:

	void handle(const DataContext& ctx) override {
		ESP_LOGI(TAG, "输出采集数据11");
		auto& sd = StorageManager::getInstance().getSDCard();
		const DataPacket& packet = ctx.packet;
		ESP_LOGI(TAG, "输出采集数据2");
		std::vector<std::string> files = sd.listDirectory("/sdcard/data");
		ESP_LOGI(TAG, "输出采集数据3");

		for (size_t i = 0; i < files.size(); ++i) {
			ESP_LOGI(TAG, "输出采集数据4");
			ESP_LOGI("TAG", "[%zu] %s", i, files[i].c_str());
		}

		// 2. 获取文件路径（按日期自动分文件）
		std::string filepath = getDailyFilePath();

		// 3. 确保表头存在（如果需要）
		if (!ensureHeaderExists(sd, filepath)) {
			ESP_LOGE(TAG, "Failed to ensure CSV header");
			// 继续尝试写入数据（表头只是可选的）
		}

		// 构建数据行
		const DeviceRecord& record = *(packet.data.record);
		std::string row = buildCSVRow(record);
		if (row.empty()) {
			ESP_LOGE(TAG, "%s", "Failed to build CSV row");
			return;
		}

		// 追加写入CSV行数据
		if (!sd.appendFile(filepath, row + "\n")) {
			ESP_LOGE(TAG, "%s", "Failed to write: ");
			return;
		}
		ESP_LOGI(TAG, "Data saved to %s", filepath.c_str());
	}
private:
	std::string buildCSVHeader() const {
		// 请根据实际的 DataContext 结构调整
		return "DeviceId,DeviceName,Timestamp";
	}

	std::string buildCSVRow(const DeviceRecord& record) const {
		std::string row =
			record.device_id + "," +
			record.device_name + "," +
			record.timestamp;

		// 添加所有数据点的值
		for (const auto& point : record.points) {
			row += "," + point.getValueString();
			row += "," + point.unit;
		}
        return row;
    }

	// ============================================================
	// 路径和日期管理
	// ============================================================

	std::string getTimestamp() const {
		time_t now = time(nullptr);
		struct tm tm_info;
		localtime_r(&now, &tm_info);

		char buffer[32];
		strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &tm_info);
		return std::string(buffer);
	}

	std::string getDateString() const {
		time_t now = time(nullptr);
		struct tm tm_info;
		localtime_r(&now, &tm_info);

		char buffer[16];
		strftime(buffer, sizeof(buffer), "%Y%m%d", &tm_info);
		return std::string(buffer);
	}

	std::string getDailyFilePath() const {
		std::string date = getDateString();
		return "/sdcard/data/" + date + ".csv";
	}

	bool ensureHeaderExists(SDCardStorage& sd, const std::string& filepath) {
		// 检查文件是否存在
		if (sd.fileExists(filepath)) {
			return true;
		}

		// 生成表头
		std::string header = buildCSVHeader();
		if (header.empty()) {
			ESP_LOGW(TAG, "Failed to build CSV header");
			return false;
		}

		// 写入表头
		if (!sd.writeFile(filepath, header + "\n")) {
			ESP_LOGW(TAG, "Failed to write CSV header: ");
			return false;
		}

		ESP_LOGI(TAG, "CSV header written to %s", filepath.c_str());
		return true;
	}

private:
};

#endif // ESP32_STORAGE_OUTPUT_HPP
