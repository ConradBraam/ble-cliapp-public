#ifndef BLE_CLIAPP_GAP_ADVERTISING_DATA_SERIALIZER_H_
#define BLE_CLIAPP_GAP_ADVERTISING_DATA_SERIALIZER_H_

#include "Serializer.h"
#include "ble/GapAdvertisingData.h"
#include "ble/UUID.h"
#include <algorithm>
#include "UUID.h"
#include "Hex.h"

#include "CLICommand/CommandArgs.h"
#include "JSONOutputStream.h"

template<>
struct SerializerDescription<GapAdvertisingData::Appearance_t> {
    typedef GapAdvertisingData::Appearance_t type;

    static const ConstArray<ValueToStringMapping<GapAdvertisingData::Appearance_t> > mapping() {
        static const ValueToStringMapping<GapAdvertisingData::Appearance_t> map[] = {
            { GapAdvertisingData::UNKNOWN, "UNKNOWN" },
            { GapAdvertisingData::GENERIC_PHONE, "GENERIC_PHONE" },
            { GapAdvertisingData::GENERIC_COMPUTER, "GENERIC_COMPUTER" },
            { GapAdvertisingData::GENERIC_WATCH, "GENERIC_WATCH" },
            { GapAdvertisingData::WATCH_SPORTS_WATCH, "WATCH_SPORTS_WATCH" },
            { GapAdvertisingData::GENERIC_CLOCK, "GENERIC_CLOCK" },
            { GapAdvertisingData::GENERIC_DISPLAY, "GENERIC_DISPLAY" },
            { GapAdvertisingData::GENERIC_REMOTE_CONTROL, "GENERIC_REMOTE_CONTROL" },
            { GapAdvertisingData::GENERIC_EYE_GLASSES, "GENERIC_EYE_GLASSES" },
            { GapAdvertisingData::GENERIC_TAG, "GENERIC_TAG" },
            { GapAdvertisingData::GENERIC_KEYRING, "GENERIC_KEYRING" },
            { GapAdvertisingData::GENERIC_MEDIA_PLAYER, "GENERIC_MEDIA_PLAYER" },
            { GapAdvertisingData::GENERIC_BARCODE_SCANNER, "GENERIC_BARCODE_SCANNER" },
            { GapAdvertisingData::GENERIC_THERMOMETER, "GENERIC_THERMOMETER" },
            { GapAdvertisingData::THERMOMETER_EAR, "THERMOMETER_EAR" },
            { GapAdvertisingData::GENERIC_HEART_RATE_SENSOR, "GENERIC_HEART_RATE_SENSOR" },
            { GapAdvertisingData::HEART_RATE_SENSOR_HEART_RATE_BELT, "HEART_RATE_SENSOR_HEART_RATE_BELT" },
            { GapAdvertisingData::GENERIC_BLOOD_PRESSURE, "GENERIC_BLOOD_PRESSURE" },
            { GapAdvertisingData::BLOOD_PRESSURE_ARM, "BLOOD_PRESSURE_ARM" },
            { GapAdvertisingData::BLOOD_PRESSURE_WRIST, "BLOOD_PRESSURE_WRIST" },
            { GapAdvertisingData::HUMAN_INTERFACE_DEVICE_HID, "HUMAN_INTERFACE_DEVICE_HID" },
            { GapAdvertisingData::KEYBOARD, "KEYBOARD" },
            { GapAdvertisingData::MOUSE, "MOUSE" },
            { GapAdvertisingData::JOYSTICK, "JOYSTICK" },
            { GapAdvertisingData::GAMEPAD, "GAMEPAD" },
            { GapAdvertisingData::DIGITIZER_TABLET, "DIGITIZER_TABLET" },
            { GapAdvertisingData::CARD_READER, "CARD_READER" },
            { GapAdvertisingData::DIGITAL_PEN, "DIGITAL_PEN" },
            { GapAdvertisingData::BARCODE_SCANNER, "BARCODE_SCANNER" },
            { GapAdvertisingData::GENERIC_GLUCOSE_METER, "GENERIC_GLUCOSE_METER" },
            { GapAdvertisingData::GENERIC_RUNNING_WALKING_SENSOR, "GENERIC_RUNNING_WALKING_SENSOR" },
            { GapAdvertisingData::RUNNING_WALKING_SENSOR_IN_SHOE, "RUNNING_WALKING_SENSOR_IN_SHOE" },
            { GapAdvertisingData::RUNNING_WALKING_SENSOR_ON_SHOE, "RUNNING_WALKING_SENSOR_ON_SHOE" },
            { GapAdvertisingData::RUNNING_WALKING_SENSOR_ON_HIP, "RUNNING_WALKING_SENSOR_ON_HIP" },
            { GapAdvertisingData::GENERIC_CYCLING, "GENERIC_CYCLING" },
            { GapAdvertisingData::CYCLING_CYCLING_COMPUTER, "CYCLING_CYCLING_COMPUTER" },
            { GapAdvertisingData::CYCLING_SPEED_SENSOR, "CYCLING_SPEED_SENSOR" },
            { GapAdvertisingData::CYCLING_CADENCE_SENSOR, "CYCLING_CADENCE_SENSOR" },
            { GapAdvertisingData::CYCLING_POWER_SENSOR, "CYCLING_POWER_SENSOR" },
            { GapAdvertisingData::CYCLING_SPEED_AND_CADENCE_SENSOR, "CYCLING_SPEED_AND_CADENCE_SENSOR" },
            { GapAdvertisingData::PULSE_OXIMETER_GENERIC, "PULSE_OXIMETER_GENERIC" },
            { GapAdvertisingData::PULSE_OXIMETER_FINGERTIP, "PULSE_OXIMETER_FINGERTIP" },
            { GapAdvertisingData::PULSE_OXIMETER_WRIST_WORN, "PULSE_OXIMETER_WRIST_WORN" },
            { GapAdvertisingData::OUTDOOR_GENERIC, "OUTDOOR_GENERIC" },
            { GapAdvertisingData::OUTDOOR_LOCATION_DISPLAY_DEVICE, "OUTDOOR_LOCATION_DISPLAY_DEVICE" },
            { GapAdvertisingData::OUTDOOR_LOCATION_AND_NAVIGATION_DISPLAY_DEVICE, "OUTDOOR_LOCATION_AND_NAVIGATION_DISPLAY_DEVICE" },
            { GapAdvertisingData::OUTDOOR_LOCATION_POD, "OUTDOOR_LOCATION_POD" },
            { GapAdvertisingData::OUTDOOR_LOCATION_AND_NAVIGATION_POD, "OUTDOOR_LOCATION_AND_NAVIGATION_POD" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown GapAdvertisingData::Appearance_t";
    }
};

static inline serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, GapAdvertisingData::Appearance_t appearance) {
    return os << toString(appearance);
}

template<>
struct SerializerDescription<GapAdvertisingData::DataType_t> {
    typedef GapAdvertisingData::DataType_t type;

    static const ConstArray<ValueToStringMapping<type> > mapping() {
        static const ValueToStringMapping<type> map[] = {
            { GapAdvertisingData::FLAGS, "FLAGS" },
            { GapAdvertisingData::INCOMPLETE_LIST_16BIT_SERVICE_IDS, "INCOMPLETE_LIST_16BIT_SERVICE_IDS" },
            { GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS, "COMPLETE_LIST_16BIT_SERVICE_IDS" },
            { GapAdvertisingData::INCOMPLETE_LIST_32BIT_SERVICE_IDS, "INCOMPLETE_LIST_32BIT_SERVICE_IDS" },
            { GapAdvertisingData::COMPLETE_LIST_32BIT_SERVICE_IDS, "COMPLETE_LIST_32BIT_SERVICE_IDS" },
            { GapAdvertisingData::INCOMPLETE_LIST_128BIT_SERVICE_IDS, "INCOMPLETE_LIST_128BIT_SERVICE_IDS" },
            { GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS, "COMPLETE_LIST_128BIT_SERVICE_IDS" },
            { GapAdvertisingData::SHORTENED_LOCAL_NAME, "SHORTENED_LOCAL_NAME" },
            { GapAdvertisingData::COMPLETE_LOCAL_NAME, "COMPLETE_LOCAL_NAME" },
            { GapAdvertisingData::TX_POWER_LEVEL, "TX_POWER_LEVEL" },
            { GapAdvertisingData::DEVICE_ID, "DEVICE_ID" },
            { GapAdvertisingData::SLAVE_CONNECTION_INTERVAL_RANGE, "SLAVE_CONNECTION_INTERVAL_RANGE" },
            { GapAdvertisingData::SERVICE_DATA, "SERVICE_DATA" },
            { GapAdvertisingData::APPEARANCE, "APPEARANCE" },
            { GapAdvertisingData::ADVERTISING_INTERVAL, "ADVERTISING_INTERVAL" },
            { GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA, "MANUFACTURER_SPECIFIC_DATA" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown GapAdvertisingData::DataType_t";
    }
};

template<>
struct SerializerDescription<GapAdvertisingData::Flags_t> {
    typedef GapAdvertisingData::Flags_t type;

    static const ConstArray<ValueToStringMapping<type> > mapping() {
        static const ValueToStringMapping<type> map[] = {
            { GapAdvertisingData::LE_LIMITED_DISCOVERABLE, "LE_LIMITED_DISCOVERABLE" },
            { GapAdvertisingData::LE_GENERAL_DISCOVERABLE, "LE_GENERAL_DISCOVERABLE" },
            { GapAdvertisingData::BREDR_NOT_SUPPORTED, "BREDR_NOT_SUPPORTED" },
            { GapAdvertisingData::SIMULTANEOUS_LE_BREDR_C, "SIMULTANEOUS_LE_BREDR_C" },
            { GapAdvertisingData::SIMULTANEOUS_LE_BREDR_H, "SIMULTANEOUS_LE_BREDR_H" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown GapAdvertisingData::Flags_t";
    }
};

/**
 * @brief Convert raw gap advertising data to dynamic::Value
 *
 * @param data The data to convert
 * @param size The length of the data
 */
serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const GapAdvertisingData& advertisingData);
struct AdvertisingDataSerializer {
    AdvertisingDataSerializer(const uint8_t* _data, uint8_t _size) : data(_data), size(_size) {}
    const uint8_t* data;
    uint8_t size;
};
serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const AdvertisingDataSerializer& advertisingData);


struct AdvertisingPayloadField_t {
    GapAdvertisingData::DataType_t dataType;
    uint8_t data[GAP_ADVERTISING_DATA_MAX_PAYLOAD];
    uint8_t dataLenght;
};

/**
 * Extract an Advertising payload field from the CLI
 * return NULL in case of success or the reason of the error
 */
const char* advertisingPayloadFieldFromCLI(const CommandArgs& args, AdvertisingPayloadField_t& payloadField);

#endif //BLE_CLIAPP_GAP_ADVERTISING_DATA_SERIALIZER_H_
