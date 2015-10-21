#ifndef BLE_CLIAPP_GAP_ADVERTISING_DATA_SERIALIZER_H_
#define BLE_CLIAPP_GAP_ADVERTISING_DATA_SERIALIZER_H_

#include "Serializer.h"
#include "ble/GapAdvertisingData.h"


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


#endif //BLE_CLIAPP_GAP_ADVERTISING_DATA_SERIALIZER_H_