idf_component_register(
    SRCS
		"src/littlefs_storage.cpp"
		"src/sdcard_storage.cpp"
    	"src/storage_manager.cpp"
    INCLUDE_DIRS
        "include"
    REQUIRES
       	littlefs                    # 替换 spiffs
        nvs_flash
        log
        esp_common
		fatfs
		vfs                      # LittleFS 同样需要 VFS
        newlib                      # 添加 newlib，提供 fopen/fread 等标准库函数
)