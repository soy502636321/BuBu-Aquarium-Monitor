// mqtt_config.h
#pragma once

#include <string>
#include <string_view>

// ============================================================================
// ★★★ MQTT 服务器连接配置 ★★★
// ============================================================================

class MqttBrokerConfig {
public:
    // 生成环境
    #ifdef defined(CONFIG_ENV_PRODUCTION)
		static constexpr std::string_view BROKER    = "mqtt://192.168.1.100:1883";
	    static constexpr int           PORT         = 8883;
	    static constexpr std::string_view USERNAME  = "your_username";
	    static constexpr std::string_view PASSWORD  = "your_password";
	    static constexpr bool SSL_ENABLED = true;
		static constexpr const char* ca_cert() {
		        return R"()";
		}
	#else
	// 默认：开发环境
		static constexpr std::string_view BROKER    = "mqtt://broker.emqx.io";
	    static constexpr int           PORT         = 1883;
	    static constexpr std::string_view USERNAME  = ""; 
	    static constexpr std::string_view PASSWORD  = "";
	    static constexpr bool SSL_ENABLED = false;
		static constexpr const char* ca_cert() {
		        return R"()";
		}
	#endif
    
    
      // -------- 通用配置 --------
	
    static constexpr int KEEPALIVE = 60;
    static constexpr int RECONNECT_TIMEOUT_MS = 5000;
    
	static std::string clientId();
    
    // -------- 其他配置 --------
    static constexpr std::string_view broker() { return BROKER; }
    static constexpr std::string_view username() { return USERNAME; }
    static constexpr std::string_view password() { return PASSWORD; }
    static constexpr bool sslEnabled() { return SSL_ENABLED; }
    static constexpr int keepalive() { return KEEPALIVE; }
    static constexpr int reconnectTimeoutMs() { return RECONNECT_TIMEOUT_MS; }
};
