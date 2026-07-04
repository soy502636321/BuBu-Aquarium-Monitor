#ifndef EEZ_LVGL_UI_STRUCTS_H
#define EEZ_LVGL_UI_STRUCTS_H

#include "eez-flow.h"

#include <stdint.h>
#include <stdbool.h>

#include "vars.h"

using namespace eez;

enum FlowStructures {
    FLOW_STRUCTURE_MESSAGE_T = 16384,
    FLOW_STRUCTURE_DEVICE_T = 16385
};

enum FlowArrayOfStructures {
    FLOW_ARRAY_OF_STRUCTURE_MESSAGE_T = 81920,
    FLOW_ARRAY_OF_STRUCTURE_DEVICE_T = 81921
};

enum message_tFlowStructureFields {
    FLOW_STRUCTURE_MESSAGE_T_FIELD_TEXT = 0,
    FLOW_STRUCTURE_MESSAGE_T_NUM_FIELDS
};

enum device_tFlowStructureFields {
    FLOW_STRUCTURE_DEVICE_T_FIELD_DEVICE_ID = 0,
    FLOW_STRUCTURE_DEVICE_T_FIELD_DEVICE_NAME = 1,
    FLOW_STRUCTURE_DEVICE_T_FIELD_DEVICE_TYPE = 2,
    FLOW_STRUCTURE_DEVICE_T_FIELD_DEVICE_ICON = 3,
    FLOW_STRUCTURE_DEVICE_T_FIELD_ACTIVE = 4,
    FLOW_STRUCTURE_DEVICE_T_NUM_FIELDS
};

struct message_tValue {
    Value value;
    
    message_tValue() {
        value = Value::makeArrayRef(FLOW_STRUCTURE_MESSAGE_T_NUM_FIELDS, FLOW_STRUCTURE_MESSAGE_T, 0);
    }
    
    message_tValue(Value value) : value(value) {}
    
    operator Value() const { return value; }
    
    operator bool() const { return value.isArray(); }
    
    const char *text() {
        return value.getArray()->values[FLOW_STRUCTURE_MESSAGE_T_FIELD_TEXT].getString();
    }
    void text(const char *text) {
        value.getArray()->values[FLOW_STRUCTURE_MESSAGE_T_FIELD_TEXT] = StringValue(text);
    }
};

typedef ArrayOf<message_tValue, FLOW_ARRAY_OF_STRUCTURE_MESSAGE_T> ArrayOfmessage_tValue;
struct device_tValue {
    Value value;
    
    device_tValue() {
        value = Value::makeArrayRef(FLOW_STRUCTURE_DEVICE_T_NUM_FIELDS, FLOW_STRUCTURE_DEVICE_T, 0);
    }
    
    device_tValue(Value value) : value(value) {}
    
    operator Value() const { return value; }
    
    operator bool() const { return value.isArray(); }
    
    int device_id() {
        return value.getArray()->values[FLOW_STRUCTURE_DEVICE_T_FIELD_DEVICE_ID].getInt();
    }
    void device_id(int device_id) {
        value.getArray()->values[FLOW_STRUCTURE_DEVICE_T_FIELD_DEVICE_ID] = IntegerValue(device_id);
    }
    
    const char *device_name() {
        return value.getArray()->values[FLOW_STRUCTURE_DEVICE_T_FIELD_DEVICE_NAME].getString();
    }
    void device_name(const char *device_name) {
        value.getArray()->values[FLOW_STRUCTURE_DEVICE_T_FIELD_DEVICE_NAME] = StringValue(device_name);
    }
    
    device_type_t device_type() {
        return (device_type_t)value.getArray()->values[FLOW_STRUCTURE_DEVICE_T_FIELD_DEVICE_TYPE].getInt();
    }
    void device_type(device_type_t device_type) {
        value.getArray()->values[FLOW_STRUCTURE_DEVICE_T_FIELD_DEVICE_TYPE] = IntegerValue((int)device_type);
    }
    
    const char *device_icon() {
        return value.getArray()->values[FLOW_STRUCTURE_DEVICE_T_FIELD_DEVICE_ICON].getString();
    }
    void device_icon(const char *device_icon) {
        value.getArray()->values[FLOW_STRUCTURE_DEVICE_T_FIELD_DEVICE_ICON] = StringValue(device_icon);
    }
    
    bool active() {
        return value.getArray()->values[FLOW_STRUCTURE_DEVICE_T_FIELD_ACTIVE].getBoolean();
    }
    void active(bool active) {
        value.getArray()->values[FLOW_STRUCTURE_DEVICE_T_FIELD_ACTIVE] = BooleanValue(active);
    }
};

typedef ArrayOf<device_tValue, FLOW_ARRAY_OF_STRUCTURE_DEVICE_T> ArrayOfdevice_tValue;

#endif /*EEZ_LVGL_UI_STRUCTS_H*/