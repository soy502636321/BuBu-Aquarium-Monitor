idf_component_register(
    SRCS
        "src/device_manager.cpp"
    INCLUDE_DIRS
        "include"
    REQUIRES
    	"event_bus"
)