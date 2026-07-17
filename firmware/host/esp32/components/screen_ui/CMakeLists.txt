file(GLOB_RECURSE SRC_FILES  "*.c" "*.cpp")

idf_component_register(
    SRCS 
    	${SRC_FILES}
    INCLUDE_DIRS
        "."
        "src/ui"
        "src/ui/fonts"
        "src/ui/images"
	REQUIRES 
        lvgl
        event_bus
		esp_wifi
		wifi_manager
		bluetooth_manager
		device_manager
)