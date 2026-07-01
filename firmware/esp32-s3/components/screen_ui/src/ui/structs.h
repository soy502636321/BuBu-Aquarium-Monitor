#ifndef EEZ_LVGL_UI_STRUCTS_H
#define EEZ_LVGL_UI_STRUCTS_H

#include "eez-flow.h"

#include <stdint.h>
#include <stdbool.h>

#include "vars.h"

using namespace eez;

enum FlowStructures {
    FLOW_STRUCTURE_MESSAGE_T = 16384
};

enum FlowArrayOfStructures {
    FLOW_ARRAY_OF_STRUCTURE_MESSAGE_T = 81920
};

enum message_tFlowStructureFields {
    FLOW_STRUCTURE_MESSAGE_T_FIELD_LEVEL = 0,
    FLOW_STRUCTURE_MESSAGE_T_NUM_FIELDS
};

struct message_tValue {
    Value value;
    
    message_tValue() {
        value = Value::makeArrayRef(FLOW_STRUCTURE_MESSAGE_T_NUM_FIELDS, FLOW_STRUCTURE_MESSAGE_T, 0);
    }
    
    message_tValue(Value value) : value(value) {}
    
    operator Value() const { return value; }
    
    operator bool() const { return value.isArray(); }
    
    message_level level() {
        return (message_level)value.getArray()->values[FLOW_STRUCTURE_MESSAGE_T_FIELD_LEVEL].getInt();
    }
    void level(message_level level) {
        value.getArray()->values[FLOW_STRUCTURE_MESSAGE_T_FIELD_LEVEL] = IntegerValue((int)level);
    }
};

typedef ArrayOf<message_tValue, FLOW_ARRAY_OF_STRUCTURE_MESSAGE_T> ArrayOfmessage_tValue;

#endif /*EEZ_LVGL_UI_STRUCTS_H*/