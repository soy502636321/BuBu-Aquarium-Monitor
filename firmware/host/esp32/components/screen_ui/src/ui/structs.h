#ifndef EEZ_LVGL_UI_STRUCTS_H
#define EEZ_LVGL_UI_STRUCTS_H

#include "eez-flow.h"

#include <stdint.h>
#include <stdbool.h>

#include "vars.h"

using namespace eez;

enum FlowStructures {
    FLOW_STRUCTURE_MESSAGE = 16384,
    FLOW_STRUCTURE_DEVICE = 16385,
    FLOW_STRUCTURE_WI_FI_RECORD = 16386,
    FLOW_STRUCTURE_BLUETOOTH_RECORD = 16387,
    FLOW_STRUCTURE_DEVICE_DATA = 16388,
    FLOW_STRUCTURE_DEVICE_PWM = 16389,
    FLOW_STRUCTURE_DEVICE_SWITCH = 16390
};

enum FlowArrayOfStructures {
    FLOW_ARRAY_OF_STRUCTURE_MESSAGE = 81920,
    FLOW_ARRAY_OF_STRUCTURE_DEVICE = 81921,
    FLOW_ARRAY_OF_STRUCTURE_WI_FI_RECORD = 81922,
    FLOW_ARRAY_OF_STRUCTURE_BLUETOOTH_RECORD = 81923,
    FLOW_ARRAY_OF_STRUCTURE_DEVICE_DATA = 81924,
    FLOW_ARRAY_OF_STRUCTURE_DEVICE_PWM = 81925,
    FLOW_ARRAY_OF_STRUCTURE_DEVICE_SWITCH = 81926
};

enum MessageFlowStructureFields {
    FLOW_STRUCTURE_MESSAGE_FIELD_CONTENT = 0,
    FLOW_STRUCTURE_MESSAGE_FIELD_TIMESTAMP = 1,
    FLOW_STRUCTURE_MESSAGE_FIELD_ACTIVE = 2,
    FLOW_STRUCTURE_MESSAGE_FIELD_STATUS = 3,
    FLOW_STRUCTURE_MESSAGE_FIELD_TYPE = 4,
    FLOW_STRUCTURE_MESSAGE_NUM_FIELDS
};

enum DeviceFlowStructureFields {
    FLOW_STRUCTURE_DEVICE_FIELD_ID = 0,
    FLOW_STRUCTURE_DEVICE_FIELD_NAME = 1,
    FLOW_STRUCTURE_DEVICE_FIELD_CATEGORY = 2,
    FLOW_STRUCTURE_DEVICE_FIELD_ICON = 3,
    FLOW_STRUCTURE_DEVICE_FIELD_ACTIVE = 4,
    FLOW_STRUCTURE_DEVICE_NUM_FIELDS
};

enum WiFiRecordFlowStructureFields {
    FLOW_STRUCTURE_WI_FI_RECORD_FIELD_SSID = 0,
    FLOW_STRUCTURE_WI_FI_RECORD_FIELD_RSSI = 1,
    FLOW_STRUCTURE_WI_FI_RECORD_FIELD_AUTHMODE = 2,
    FLOW_STRUCTURE_WI_FI_RECORD_FIELD_ACTIVE = 3,
    FLOW_STRUCTURE_WI_FI_RECORD_NUM_FIELDS
};

enum BluetoothRecordFlowStructureFields {
    FLOW_STRUCTURE_BLUETOOTH_RECORD_FIELD_NAME = 0,
    FLOW_STRUCTURE_BLUETOOTH_RECORD_FIELD_RSSI = 1,
    FLOW_STRUCTURE_BLUETOOTH_RECORD_FIELD_ACTIVE = 2,
    FLOW_STRUCTURE_BLUETOOTH_RECORD_NUM_FIELDS
};

enum DeviceDataFlowStructureFields {
    FLOW_STRUCTURE_DEVICE_DATA_FIELD_TITLE = 0,
    FLOW_STRUCTURE_DEVICE_DATA_FIELD_UNIT = 1,
    FLOW_STRUCTURE_DEVICE_DATA_FIELD_MEASUREMENT = 2,
    FLOW_STRUCTURE_DEVICE_DATA_FIELD_STATUS = 3,
    FLOW_STRUCTURE_DEVICE_DATA_FIELD_TIMESTAMP = 4,
    FLOW_STRUCTURE_DEVICE_DATA_FIELD_ACTIVE = 5,
    FLOW_STRUCTURE_DEVICE_DATA_NUM_FIELDS
};

enum DevicePwmFlowStructureFields {
    FLOW_STRUCTURE_DEVICE_PWM_FIELD_TITLE = 0,
    FLOW_STRUCTURE_DEVICE_PWM_FIELD_ACTIVE = 1,
    FLOW_STRUCTURE_DEVICE_PWM_NUM_FIELDS
};

enum DeviceSwitchFlowStructureFields {
    FLOW_STRUCTURE_DEVICE_SWITCH_FIELD_TITLE = 0,
    FLOW_STRUCTURE_DEVICE_SWITCH_FIELD_ACTIVE = 1,
    FLOW_STRUCTURE_DEVICE_SWITCH_NUM_FIELDS
};

struct MessageValue {
    Value value;
    
    MessageValue() {
        value = Value::makeArrayRef(FLOW_STRUCTURE_MESSAGE_NUM_FIELDS, FLOW_STRUCTURE_MESSAGE, 0);
    }
    
    MessageValue(Value value) : value(value) {}
    
    operator Value() const { return value; }
    
    operator bool() const { return value.isArray(); }
    
    const char *content() {
        return value.getArray()->values[FLOW_STRUCTURE_MESSAGE_FIELD_CONTENT].getString();
    }
    void content(const char *content) {
        value.getArray()->values[FLOW_STRUCTURE_MESSAGE_FIELD_CONTENT] = StringValue(content);
    }
    
    const char *timestamp() {
        return value.getArray()->values[FLOW_STRUCTURE_MESSAGE_FIELD_TIMESTAMP].getString();
    }
    void timestamp(const char *timestamp) {
        value.getArray()->values[FLOW_STRUCTURE_MESSAGE_FIELD_TIMESTAMP] = StringValue(timestamp);
    }
    
    bool active() {
        return value.getArray()->values[FLOW_STRUCTURE_MESSAGE_FIELD_ACTIVE].getBoolean();
    }
    void active(bool active) {
        value.getArray()->values[FLOW_STRUCTURE_MESSAGE_FIELD_ACTIVE] = BooleanValue(active);
    }
    
    MessageStatus status() {
        return (MessageStatus)value.getArray()->values[FLOW_STRUCTURE_MESSAGE_FIELD_STATUS].getInt();
    }
    void status(MessageStatus status) {
        value.getArray()->values[FLOW_STRUCTURE_MESSAGE_FIELD_STATUS] = IntegerValue((int)status);
    }
    
    MessageType type() {
        return (MessageType)value.getArray()->values[FLOW_STRUCTURE_MESSAGE_FIELD_TYPE].getInt();
    }
    void type(MessageType type) {
        value.getArray()->values[FLOW_STRUCTURE_MESSAGE_FIELD_TYPE] = IntegerValue((int)type);
    }
};

typedef ArrayOf<MessageValue, FLOW_ARRAY_OF_STRUCTURE_MESSAGE> ArrayOfMessageValue;
struct DeviceValue {
    Value value;
    
    DeviceValue() {
        value = Value::makeArrayRef(FLOW_STRUCTURE_DEVICE_NUM_FIELDS, FLOW_STRUCTURE_DEVICE, 0);
    }
    
    DeviceValue(Value value) : value(value) {}
    
    operator Value() const { return value; }
    
    operator bool() const { return value.isArray(); }
    
    int id() {
        return value.getArray()->values[FLOW_STRUCTURE_DEVICE_FIELD_ID].getInt();
    }
    void id(int id) {
        value.getArray()->values[FLOW_STRUCTURE_DEVICE_FIELD_ID] = IntegerValue(id);
    }
    
    const char *name() {
        return value.getArray()->values[FLOW_STRUCTURE_DEVICE_FIELD_NAME].getString();
    }
    void name(const char *name) {
        value.getArray()->values[FLOW_STRUCTURE_DEVICE_FIELD_NAME] = StringValue(name);
    }
    
    DeviceCategory category() {
        return (DeviceCategory)value.getArray()->values[FLOW_STRUCTURE_DEVICE_FIELD_CATEGORY].getInt();
    }
    void category(DeviceCategory category) {
        value.getArray()->values[FLOW_STRUCTURE_DEVICE_FIELD_CATEGORY] = IntegerValue((int)category);
    }
    
    const char *icon() {
        return value.getArray()->values[FLOW_STRUCTURE_DEVICE_FIELD_ICON].getString();
    }
    void icon(const char *icon) {
        value.getArray()->values[FLOW_STRUCTURE_DEVICE_FIELD_ICON] = StringValue(icon);
    }
    
    bool active() {
        return value.getArray()->values[FLOW_STRUCTURE_DEVICE_FIELD_ACTIVE].getBoolean();
    }
    void active(bool active) {
        value.getArray()->values[FLOW_STRUCTURE_DEVICE_FIELD_ACTIVE] = BooleanValue(active);
    }
};

typedef ArrayOf<DeviceValue, FLOW_ARRAY_OF_STRUCTURE_DEVICE> ArrayOfDeviceValue;
struct WiFiRecordValue {
    Value value;
    
    WiFiRecordValue() {
        value = Value::makeArrayRef(FLOW_STRUCTURE_WI_FI_RECORD_NUM_FIELDS, FLOW_STRUCTURE_WI_FI_RECORD, 0);
    }
    
    WiFiRecordValue(Value value) : value(value) {}
    
    operator Value() const { return value; }
    
    operator bool() const { return value.isArray(); }
    
    const char *ssid() {
        return value.getArray()->values[FLOW_STRUCTURE_WI_FI_RECORD_FIELD_SSID].getString();
    }
    void ssid(const char *ssid) {
        value.getArray()->values[FLOW_STRUCTURE_WI_FI_RECORD_FIELD_SSID] = StringValue(ssid);
    }
    
    int rssi() {
        return value.getArray()->values[FLOW_STRUCTURE_WI_FI_RECORD_FIELD_RSSI].getInt();
    }
    void rssi(int rssi) {
        value.getArray()->values[FLOW_STRUCTURE_WI_FI_RECORD_FIELD_RSSI] = IntegerValue(rssi);
    }
    
    int authmode() {
        return value.getArray()->values[FLOW_STRUCTURE_WI_FI_RECORD_FIELD_AUTHMODE].getInt();
    }
    void authmode(int authmode) {
        value.getArray()->values[FLOW_STRUCTURE_WI_FI_RECORD_FIELD_AUTHMODE] = IntegerValue(authmode);
    }
    
    bool active() {
        return value.getArray()->values[FLOW_STRUCTURE_WI_FI_RECORD_FIELD_ACTIVE].getBoolean();
    }
    void active(bool active) {
        value.getArray()->values[FLOW_STRUCTURE_WI_FI_RECORD_FIELD_ACTIVE] = BooleanValue(active);
    }
};

typedef ArrayOf<WiFiRecordValue, FLOW_ARRAY_OF_STRUCTURE_WI_FI_RECORD> ArrayOfWiFiRecordValue;
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
struct DeviceDataValue {
    Value value;
    
    DeviceDataValue() {
        value = Value::makeArrayRef(FLOW_STRUCTURE_DEVICE_DATA_NUM_FIELDS, FLOW_STRUCTURE_DEVICE_DATA, 0);
    }
    
    DeviceDataValue(Value value) : value(value) {}
    
    operator Value() const { return value; }
    
    operator bool() const { return value.isArray(); }
    
    const char *title() {
        return value.getArray()->values[FLOW_STRUCTURE_DEVICE_DATA_FIELD_TITLE].getString();
    }
    void title(const char *title) {
        value.getArray()->values[FLOW_STRUCTURE_DEVICE_DATA_FIELD_TITLE] = StringValue(title);
    }
    
    const char *unit() {
        return value.getArray()->values[FLOW_STRUCTURE_DEVICE_DATA_FIELD_UNIT].getString();
    }
    void unit(const char *unit) {
        value.getArray()->values[FLOW_STRUCTURE_DEVICE_DATA_FIELD_UNIT] = StringValue(unit);
    }
    
    double measurement() {
        return value.getArray()->values[FLOW_STRUCTURE_DEVICE_DATA_FIELD_MEASUREMENT].getDouble();
    }
    void measurement(double measurement) {
        value.getArray()->values[FLOW_STRUCTURE_DEVICE_DATA_FIELD_MEASUREMENT] = DoubleValue(measurement);
    }
    
    DeviceDataStatus status() {
        return (DeviceDataStatus)value.getArray()->values[FLOW_STRUCTURE_DEVICE_DATA_FIELD_STATUS].getInt();
    }
    void status(DeviceDataStatus status) {
        value.getArray()->values[FLOW_STRUCTURE_DEVICE_DATA_FIELD_STATUS] = IntegerValue((int)status);
    }
    
    Value timestamp() {
        return value.getArray()->values[FLOW_STRUCTURE_DEVICE_DATA_FIELD_TIMESTAMP];
    }
    void timestamp(Value timestamp) {
        value.getArray()->values[FLOW_STRUCTURE_DEVICE_DATA_FIELD_TIMESTAMP] = timestamp;
    }
    
    bool active() {
        return value.getArray()->values[FLOW_STRUCTURE_DEVICE_DATA_FIELD_ACTIVE].getBoolean();
    }
    void active(bool active) {
        value.getArray()->values[FLOW_STRUCTURE_DEVICE_DATA_FIELD_ACTIVE] = BooleanValue(active);
    }
};

typedef ArrayOf<DeviceDataValue, FLOW_ARRAY_OF_STRUCTURE_DEVICE_DATA> ArrayOfDeviceDataValue;
struct DevicePwmValue {
    Value value;
    
    DevicePwmValue() {
        value = Value::makeArrayRef(FLOW_STRUCTURE_DEVICE_PWM_NUM_FIELDS, FLOW_STRUCTURE_DEVICE_PWM, 0);
    }
    
    DevicePwmValue(Value value) : value(value) {}
    
    operator Value() const { return value; }
    
    operator bool() const { return value.isArray(); }
    
    const char *title() {
        return value.getArray()->values[FLOW_STRUCTURE_DEVICE_PWM_FIELD_TITLE].getString();
    }
    void title(const char *title) {
        value.getArray()->values[FLOW_STRUCTURE_DEVICE_PWM_FIELD_TITLE] = StringValue(title);
    }
    
    bool active() {
        return value.getArray()->values[FLOW_STRUCTURE_DEVICE_PWM_FIELD_ACTIVE].getBoolean();
    }
    void active(bool active) {
        value.getArray()->values[FLOW_STRUCTURE_DEVICE_PWM_FIELD_ACTIVE] = BooleanValue(active);
    }
};

typedef ArrayOf<DevicePwmValue, FLOW_ARRAY_OF_STRUCTURE_DEVICE_PWM> ArrayOfDevicePwmValue;
struct DeviceSwitchValue {
    Value value;
    
    DeviceSwitchValue() {
        value = Value::makeArrayRef(FLOW_STRUCTURE_DEVICE_SWITCH_NUM_FIELDS, FLOW_STRUCTURE_DEVICE_SWITCH, 0);
    }
    
    DeviceSwitchValue(Value value) : value(value) {}
    
    operator Value() const { return value; }
    
    operator bool() const { return value.isArray(); }
    
    const char *title() {
        return value.getArray()->values[FLOW_STRUCTURE_DEVICE_SWITCH_FIELD_TITLE].getString();
    }
    void title(const char *title) {
        value.getArray()->values[FLOW_STRUCTURE_DEVICE_SWITCH_FIELD_TITLE] = StringValue(title);
    }
    
    bool active() {
        return value.getArray()->values[FLOW_STRUCTURE_DEVICE_SWITCH_FIELD_ACTIVE].getBoolean();
    }
    void active(bool active) {
        value.getArray()->values[FLOW_STRUCTURE_DEVICE_SWITCH_FIELD_ACTIVE] = BooleanValue(active);
    }
};

typedef ArrayOf<DeviceSwitchValue, FLOW_ARRAY_OF_STRUCTURE_DEVICE_SWITCH> ArrayOfDeviceSwitchValue;

#endif /*EEZ_LVGL_UI_STRUCTS_H*/