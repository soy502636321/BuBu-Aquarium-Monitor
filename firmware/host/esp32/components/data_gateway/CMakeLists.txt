idf_component_register(
    SRCS
       	"src/data_gateway.cpp"
        "src/data_receiver.cpp"

		"include/processors/hex_to_packet_processor.hpp"
		"include/processors/packet_validator_processor.hpp"

        "src/protocol_parser.cpp"
        "src/ttl_protocol.cpp"
        "src/ble_protocol.cpp"
    INCLUDE_DIRS
        "include"
    REQUIRES
        driver
        nvs_flash
        log
        esp_common
        vfs
        bt
        device_manager
		common
)