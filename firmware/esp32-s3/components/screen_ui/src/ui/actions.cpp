#include "eez-flow.h"
#include "actions.h"
#include "vars.h"
#include <cstdio>

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
    int32_t content_page = page_value.getInt32();
    set_var_content_page(content_page);
    printf("value=%ld\n", content_page);
}

void action_router_back(lv_event_t *e) {
}