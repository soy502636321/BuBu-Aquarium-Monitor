#ifndef EEZ_LVGL_UI_STRUCTS_H
#define EEZ_LVGL_UI_STRUCTS_H

#include "eez-flow.h"

#include <stdint.h>
#include <stdbool.h>

#include "vars.h"

using namespace eez;

enum FlowStructures {
    FLOW_STRUCTURE_MESSAGE_T = 16384,
    FLOW_STRUCTURE_DEVICE_T = 16385,
    FLOW_STRUCTURE_WI_FI_RECORD_T = 16386,
    FLOW_STRUCTURE_BLUETOOTH_RECORD = 16387
};

enum FlowArrayOfStructures {
    FLOW_ARRAY_OF_STRUCTURE_MESSAGE_T = 81920,
    FLOW_ARRAY_OF_STRUCTURE_DEVICE_T = 81921,
    FLOW_ARRAY_OF_STRUCTURE_WI_FI_RECORD_T = 81922,
    FLOW_ARRAY_OF_STRUCTURE_BLUETOOTH_RECORD = 81923
};

enum Message_tFlowStructureFields {
    FLOW_STRUCTURE_MESSAGE_T_FIELD_TEXT = 0,
    FLOW_STRUCTURE_MESSAGE_T_NUM_FIELDS
};

enum Device_tFlowStructureFields {
    FLOW_STRUCTURE_DEVICE_T_FIELD_DEVICE_ID = 0,
    FLOW_STRUCTURE_DEVICE_T_FIELD_DEVICE_NAME = 1,
    FLOW_STRUCTURE_DEVICE_T_FIELD_DEVICE_TYPE = 2,
    FLOW_STRUCTURE_DEVICE_T_FIELD_DEVICE_ICON = 3,
    FLOW_STRUCTURE_DEVICE_T_FIELD_ACTIVE = 4,
    FLOW_STRUCTURE_DEVICE_T_NUM_FIELDS
};

enum WiFi_Record_tFlowStructureFields {
    FLOW_STRUCTURE_WI_FI_RECORD_T_FIELD_SSID = 0,
    FLOW_STRUCTURE_WI_FI_RECORD_T_FIELD_RSSI = 1,
    FLOW_STRUCTURE_WI_FI_RECORD_T_FIELD_AUTHMODE = 2,
    FLOW_STRUCTURE_WI_FI_RECORD_T_FIELD_ACTIVE = 3,
    FLOW_STRUCTURE_WI_FI_RECORD_T_NUM_FIELDS
};

enum BluetoothRecordFlowStructureFields {
    FLOW_STRUCTURE_BLUETOOTH_RECORD_FIELD_NAME = 0,
    FLOW_STRUCTURE_BLUETOOTH_RECORD_FIELD_RSSI = 1,
    FLOW_STRUCTURE_BLUETOOTH_RECORD_FIELD_ACTIVE = 2,
    FLOW_STRUCTURE_BLUETOOTH_RECORD_NUM_FIELDS
};

struct Message_tValue {
    Value value;
    
    Message_tValue() {
        value = Value::makeArrayRef(FLOW_STRUCTURE_MESSAGE_T_NUM_FIELDS, FLOW_STRUCTURE_MESSAGE_T, 0);
    }
    
    Message_tValue(Value value) : value(value) {}
    
    operator Value() const { return value; }
    
    operator bool() const { return value.isArray(); }
    
    const char *text() {
        return value.getArray()->values[FLOW_STRUCTURE_MESSAGE_T_FIELD_TEXT].getString();
    }
    void text(const char *text) {
        value.getArray()->values[FLOW_STRUCTURE_MESSAGE_T_FIELD_TEXT] = StringValue(text);
    }
};

typedef ArrayOf<Message_tValue, FLOW_ARRAY_OF_STRUCTURE_MESSAGE_T> ArrayOfMessage_tValue;
struct Device_tValue {
    Value value;
    
    Device_tValue() {
        value = Value::makeArrayRef(FLOW_STRUCTURE_DEVICE_T_NUM_FIELDS, FLOW_STRUCTURE_DEVICE_T, 0);
    }
    
    Device_tValue(Value value) : value(value) {}
    
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
    
    Device_Type_t device_type() {
        return (Device_Type_t)value.getArray()->values[FLOW_STRUCTURE_DEVICE_T_FIELD_DEVICE_TYPE].getInt();
    }
    void device_type(Device_Type_t device_type) {
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

typedef ArrayOf<Device_tValue, FLOW_ARRAY_OF_STRUCTURE_DEVICE_T> ArrayOfDevice_tValue;
struct WiFi_Record_tValue {
    Value value;
    
    WiFi_Record_tValue() {
        value = Value::makeArrayRef(FLOW_STRUCTURE_WI_FI_RECORD_T_NUM_FIELDS, FLOW_STRUCTURE_WI_FI_RECORD_T, 0);
    }
    
    WiFi_Record_tValue(Value value) : value(value) {}
    
    operator Value() const { return value; }
    
    operator bool() const { return value.isArray(); }
    
    const char *ssid() {
        return value.getArray()->values[FLOW_STRUCTURE_WI_FI_RECORD_T_FIELD_SSID].getString();
    }
    void ssid(const char *ssid) {
        value.getArray()->values[FLOW_STRUCTURE_WI_FI_RECORD_T_FIELD_SSID] = StringValue(ssid);
    }
    
    int rssi() {
        return value.getArray()->values[FLOW_STRUCTURE_WI_FI_RECORD_T_FIELD_RSSI].getInt();
    }
    void rssi(int rssi) {
        value.getArray()->values[FLOW_STRUCTURE_WI_FI_RECORD_T_FIELD_RSSI] = IntegerValue(rssi);
    }
    
    int authmode() {
        return value.getArray()->values[FLOW_STRUCTURE_WI_FI_RECORD_T_FIELD_AUTHMODE].getInt();
    }
    void authmode(int authmode) {
        value.getArray()->values[FLOW_STRUCTURE_WI_FI_RECORD_T_FIELD_AUTHMODE] = IntegerValue(authmode);
    }
    
    bool active() {
        return value.getArray()->values[FLOW_STRUCTURE_WI_FI_RECORD_T_FIELD_ACTIVE].getBoolean();
    }
    void active(bool active) {
        value.getArray()->values[FLOW_STRUCTURE_WI_FI_RECORD_T_FIELD_ACTIVE] = BooleanValue(active);
    }
};

typedef ArrayOf<WiFi_Record_tValue, FLOW_ARRAY_OF_STRUCTURE_WI_FI_RECORD_T> ArrayOfWiFi_Record_tValue;
struct BluetoothRecordValue {
    Value value;
    
    BluetoothRecordValue() {
        value = Value::makeArrayRef(FLOW_STRUCTURE_BLUETOOTH_RECORD_NUM_FIELDS, FLOW_STRUCTURE_BLUETOOTH_RECORD, 0);
    }
    
    BluetoothRecordValue(Value value) : value(value) {}
    
    operator Value() const { return value; }
    
    operator bool() const { return value.isArray(); }
    
    const char *name() {
        return value.getArray()->values[FLOW_STRUCTURE_BLUETOOTH_RECORD_FIELD_NAME].getString();
    }
    void name(const char *name) {
        value.getArray()->values[FLOW_STRUCTURE_BLUETOOTH_RECORD_FIELD_NAME] = StringValue(name);
    }
    
    int rssi() {
        return value.getArray()->values[FLOW_STRUCTURE_BLUETOOTH_RECORD_FIELD_RSSI].getInt();
    }
    void rssi(int rssi) {
        value.getArray()->values[FLOW_STRUCTURE_BLUETOOTH_RECORD_FIELD_RSSI] = IntegerValue(rssi);
    }
    
    bool active() {
        return value.getArray()->values[FLOW_STRUCTURE_BLUETOOTH_RECORD_FIELD_ACTIVE].getBoolean();
    }
    void active(bool active) {
        value.getArray()->values[FLOW_STRUCTURE_BLUETOOTH_RECORD_FIELD_ACTIVE] = BooleanValue(active);
    }
};

typedef ArrayOf<BluetoothRecordValue, FLOW_ARRAY_OF_STRUCTURE_BLUETOOTH_RECORD> ArrayOfBluetoothRecordValue;

#endif /*EEZ_LVGL_UI_STRUCTS_H*/