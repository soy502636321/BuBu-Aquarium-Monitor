#include "eez-flow.h"
#include "actions.h"
#include "vars.h"
#include "router.h"
#include <cstdio>
#include <stdint.h>
#include "esp_wifi.h"

void action_home_click(lv_event_t *e)
{
	printf("Hello World!");
}

void action_router_push(lv_event_t *e) {
	using namespace eez;
	using namespace eez::flow;
	printf("Router Push!");
    Value page_value = eez::flow::getUserProperty(ACTION_ROUTER_PUSH_PROPERTY_PAGE);
    //eez::flow::AsyncAction *asyncAction = eez::flow::beginAsyncExecution();
    int32_t page_id = page_value.getInt32();
    RouterPage_t page = {
	    .page_id = static_cast<content_page_t>(page_id),
	    .argument = nullptr
	};
    
    Router::instance().push(page);
    printf("value=%ld\n", page_id);
}

void action_router_back(lv_event_t *e) {
	printf("Router Back。。。");
	Router::instance().pop();
}

void action_device_firmware_ota(lv_event_t * e) {
	printf("action_device_firmware_ota Click");
}

void action_wifi_scan_start(lv_event_t * e) {
	printf("Start Scan WiFi......");
	// esp_wifi_scan_start(NULL, false);
}

void action_printf_hw(lv_event_t * e) {
	printf("Hello World!");	
}