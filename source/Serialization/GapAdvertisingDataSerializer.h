#ifndef BLE_CLIAPP_GAP_ADVERTISING_DATA_SERIALIZER_H_
#define BLE_CLIAPP_GAP_ADVERTISING_DATA_SERIALIZER_H_

#include "Serializer.h"
#include "ble/GapAdvertisingData.h"
#include "ble/UUID.h"
#include "util/picojson.h"
#include <algorithm>

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

static inline picojson::value gapAdvertisingDataToJSON(const uint8_t* data, uint8_t size) {
	picojson::value result = picojson::value(picojson::object_type, true);

	for(size_t i = 0; i < size; i = (i + data[i] + 1)) {
		if(data[i] == 0) {
			continue;
		}

		// Setup field access
		GapAdvertisingData::DataType_t dataType = (GapAdvertisingData::DataType_t) data[i + 1];
		const uint8_t* fieldData = (data[i] == 1) ? NULL : data + i + 2;
		uint8_t dataLenght = data[i] - 1;

		picojson::value fieldValue = picojson::value("conversion not handled");;

		// serialization 
		switch(dataType) { 
			case GapAdvertisingData::FLAGS: {
				// if the flags are malformed, we signal the error 
				if(dataLenght != 1) {
					// TODO : add the true value of the field 
					fieldValue = picojson::value("malformed");
					break;
				}

				// construct an array of the flags 
				fieldValue = picojson::value(picojson::array_type, true);

				const ConstArray<ValueToStringMapping<GapAdvertisingData::Flags_t> > mapping = SerializerDescription<GapAdvertisingData::Flags_t>::mapping();
				GapAdvertisingData::Flags_t flags = (GapAdvertisingData::Flags_t) fieldData[0];

				for(size_t j = 0; j < mapping.count(); ++j) {
					if(flags & mapping[j].value) {
						fieldValue.get<picojson::array>().push_back(picojson::value(mapping[j].str));
					}
				}

				break;
			}

			case GapAdvertisingData::INCOMPLETE_LIST_16BIT_SERVICE_IDS:
			case GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS:
				// if the flags are malformed, we signal the error 
				if(dataLenght % sizeof(uint16_t)) {
					// TODO : add the true value of the field 
					fieldValue = picojson::value("malformed");
					break;
				}

				// construct an array of the service ids 
				fieldValue = picojson::value(picojson::array_type, true);				
				for(size_t j = 0; j < dataLenght; j += sizeof(uint16_t)) {
					uint16_t uuid = 0;
					memcpy(&uuid, fieldData + j, sizeof(uuid));
					char uuidStr[2 + sizeof(uint16_t) * 2 + 1] = { 0 };
					snprintf(uuidStr, sizeof(uuidStr), "0x%04X", uuid);
					fieldValue.get<picojson::array>().push_back(picojson::value(uuidStr));
				}
				break;

			case GapAdvertisingData::INCOMPLETE_LIST_32BIT_SERVICE_IDS:
			case GapAdvertisingData::COMPLETE_LIST_32BIT_SERVICE_IDS:
				// if the flags are malformed, we signal the error 
				if(dataLenght % sizeof(uint32_t)) {
					// TODO : add the true value of the field 
					fieldValue = picojson::value("malformed");
					break;
				}

				// construct an array of the service ids 
				fieldValue = picojson::value(picojson::array_type, true);				
				for(size_t j = 0; j < dataLenght; j += sizeof(uint32_t)) {
					uint32_t uuid = 0;
					memcpy(&uuid, fieldData + j, sizeof(uuid));
					char uuidStr[2 + sizeof(uint32_t) * 2 + 1] = { 0 };
					snprintf(uuidStr, sizeof(uuidStr), "0x%08lX", uuid);
					fieldValue.get<picojson::array>().push_back(picojson::value(uuidStr));
				}
				break;

			case GapAdvertisingData::INCOMPLETE_LIST_128BIT_SERVICE_IDS:
			case GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS:
				break;

			case GapAdvertisingData::SHORTENED_LOCAL_NAME:
			case GapAdvertisingData::COMPLETE_LOCAL_NAME:
				if(dataLenght == 0) {
					// TODO : add the true value of the field 
					fieldValue = picojson::value("");
					break;
				}

				fieldValue = picojson::value(std::string((const char*) fieldData, dataLenght));
				break;

			case GapAdvertisingData::TX_POWER_LEVEL:
				break;

			case GapAdvertisingData::DEVICE_ID:
				break;

			case GapAdvertisingData::SLAVE_CONNECTION_INTERVAL_RANGE:
				break;

			case GapAdvertisingData::SERVICE_DATA:
				break;

			case GapAdvertisingData::APPEARANCE:
				break;

			case GapAdvertisingData::ADVERTISING_INTERVAL:
				break;

			case GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA:
				break;
		}

		result.get<picojson::object>()[toString(dataType)] = fieldValue;
	}

	char rawData[(GAP_ADVERTISING_DATA_MAX_PAYLOAD * 2) + 1] = { 0 };

	for(size_t i = 0; i < size; ++i) {
		snprintf(rawData + (i * 2), 3, "%02X", data[i]);
	}

	result.get<picojson::object>()["raw"] = picojson::value(rawData);

	return result;
}


// TODO : data to string as hex ...
static inline picojson::value gapAdvertisingDataToJSON(const GapAdvertisingData& advertisingData) {
	return gapAdvertisingDataToJSON(advertisingData.getPayload(), advertisingData.getPayloadLen());
}

// todo : better than this
static inline bool hexToChar(char msb, char lsb, uint8_t& result) {
	char hexStr[] = { msb, lsb, 0 };
	char* end;

	long conversionResult = strtol(hexStr, &end, 16);
	if(end == hexStr) {
		return false;
	}

	result = (uint8_t) conversionResult;
	return true;
}

static inline bool UUIDfromString(const char* str, UUID::LongUUIDBytes_t& uuid) {
	static const char refUUID[] = "XXXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX";
	if(strlen(str) != strlen(refUUID)) {
		return false;
	}

	UUID::LongUUIDBytes_t tmp = { 0 };
	for(size_t i = 0, currentStrIndex = 0; i < sizeof(tmp); ++i) {
		// check for separator
		if(currentStrIndex == 8 || currentStrIndex == 13 || currentStrIndex == 18 || currentStrIndex == 23) {
			if(str[currentStrIndex] != '-') {
				return false;
			}
			++currentStrIndex;
		}

		uint8_t convertedByte = 0;
		if(!hexToChar(str[currentStrIndex], str[currentStrIndex + 1], convertedByte)) {
			return false;
		}

		tmp[i] = convertedByte;
		currentStrIndex += 2;
	}

	memcpy(uuid, tmp, UUID::LENGTH_OF_LONG_UUID);
	// copy in reverse order 
	std::reverse(uuid, uuid + sizeof(uuid));

	return true;
}

struct AdvertisingPayloadField_t {
	GapAdvertisingData::DataType_t dataType;
	uint8_t data[GAP_ADVERTISING_DATA_MAX_PAYLOAD];
	uint8_t dataLenght;
};

/** 
 * return NULL in case of success or the reason of the error 
 */
static inline const char* advertisingPayloadFieldFromCLI(const CommandArgs& args, AdvertisingPayloadField_t& payloadField) {
	// first we check that at least two arguments are provided 
	if(args.count() < 2) {
		return "at least two parameters should be provided <GapAdvertisingData::DataType_t> <dataToSet>";
	}

	GapAdvertisingData::DataType_t dataType = (GapAdvertisingData::DataType_t) 0;
	if(!fromString(args[0], dataType)) {
		return "The payload data type in invalid, please refer to GapAdvertisingData::DataType_t";
	}

	uint8_t data[GAP_ADVERTISING_DATA_MAX_PAYLOAD] = { 0 };
	uint8_t dataLenght = 0;

	switch(dataType) {
		case GapAdvertisingData::FLAGS: {
			int flags = (GapAdvertisingData::Flags_t) 0;
			for(size_t i = 0; i < args.count() - 1; ++i) {
				GapAdvertisingData::Flags_t flag;
				if(!fromString(args[i + 1], flag)) {
					return "invalid flag parameter, refer to GapAdvertisingData::Flags_t";
				}
				flags |= flag;
			}
			memcpy(data, &flags, 1);
			dataLenght = 1;
			break;	
		} 

		case GapAdvertisingData::INCOMPLETE_LIST_16BIT_SERVICE_IDS:
		case GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS: {
			uint16_t services[(GAP_ADVERTISING_DATA_MAX_PAYLOAD - 2) / sizeof(uint16_t)];

			if((args.count() - 1) > (sizeof(services) / (sizeof(uint16_t)))) {
				return "too many services";
			}

			for(size_t i = 0; i < args.count() - 1; ++i) {
				uint16_t service;
				if(!fromString(args[i + 1], service)) { 
					return "malformed service";
				}
				services[i] = service;
				dataLenght+= sizeof(service);
			}

			memcpy(data, services, dataLenght);
			break;
		}

		case GapAdvertisingData::INCOMPLETE_LIST_32BIT_SERVICE_IDS:
		case GapAdvertisingData::COMPLETE_LIST_32BIT_SERVICE_IDS:{
			uint32_t services[(GAP_ADVERTISING_DATA_MAX_PAYLOAD - 2) / sizeof(uint32_t)];

			if((args.count() - 1) > (sizeof(services) / (sizeof(uint32_t)))) {
				return "too many services";
			}

			for(size_t i = 0; i < args.count() - 1; ++i) {
				uint32_t service;
				if(!fromString(args[i + 1], service)) { 
					return "malformed service";
				}
				services[i] = service;
				dataLenght+= sizeof(service);
			}

			memcpy(data, services, dataLenght);
			break;
		}

		case GapAdvertisingData::INCOMPLETE_LIST_128BIT_SERVICE_IDS:
		case GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS: {
			// it is not possible to have more than one 128 bit UUID in the payload
			if(args.count() > 2) {
				return "Too much UUIDs for the payload";
			}

			UUID::LongUUIDBytes_t uuid;
			if(!UUIDfromString(args[1], uuid)) {
				return "The uuid is not properly formated (XXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX)";
			}

			memcpy(data, uuid, sizeof(uuid));
			dataLenght = sizeof(uuid);
			break;
		}

		case GapAdvertisingData::SHORTENED_LOCAL_NAME:
		case GapAdvertisingData::COMPLETE_LOCAL_NAME:
			if(args.count() != 2) {
				return "just a name without space is expected";
			}

			if(strlen(args[1]) > sizeof(data)) {
				return "The name provided is too long";
			}

			dataLenght = strlen(args[1]);
			memcpy(data, args[1], dataLenght);
			break;

		case GapAdvertisingData::TX_POWER_LEVEL: {
			if(args.count() != 2) {
				return "too many arguments, only the TX power level is expected";
			}

			int8_t txPower;
			if(!fromString(args[1], txPower)) {
				return "the TX power level is ill formed";
			}

			memcpy(data, &txPower, sizeof(txPower));
			dataLenght = sizeof(txPower);
			break; 
		} 

		case GapAdvertisingData::DEVICE_ID:
			return "Not yet implemented for this kind of data types";

		case GapAdvertisingData::SLAVE_CONNECTION_INTERVAL_RANGE: {
			if(args.count() != 3) {
				return "<minimumConnectionInterval> <maximumConnectionInterval> needed";
			}			

			uint16_t intervals[2] = { 0 };

			if(!fromString(args[1], intervals[0])) {
				return "the minimum connection interval should be a number";
			}

			if(!fromString(args[2], intervals[1])) {
				return "the maximum connection interval should be a number";
			}

			memcpy(data, intervals, sizeof(intervals));
			dataLenght = sizeof(intervals);
			break;
		}

		case GapAdvertisingData::SERVICE_DATA:
			/*
			 UUID (16 | 32 | 128) FOLLOWED by data
			*/
			return "Not yet implemented for this kind of data types";
		
		case GapAdvertisingData::APPEARANCE: {
			if(args.count() != 2) {
				return "too many arguments, only the appearance is expected";
			}

			GapAdvertisingData::Appearance_t appearance = GapAdvertisingData::UNKNOWN;
			if(!fromString(args[1], appearance)) {
				return "appearance is malformed see (GapAdvertisingData::Appearance_t)";
			}

			// take care, appearance is only on 2 bytes 
			memcpy(data, &appearance, sizeof(uint16_t));
			dataLenght = sizeof(uint16_t);
			break;
		}


		case GapAdvertisingData::ADVERTISING_INTERVAL: {
			if(args.count() != 2) {
				return "too many arguments, only the advertising interval is expected";
			}			

			uint16_t advertisingInterval = 0;
			if(!fromString(args[1], advertisingInterval)) {
				return "the advertising interval should be a number";
			}

			memcpy(data, &advertisingInterval, sizeof(advertisingInterval));
			dataLenght = sizeof(advertisingInterval);
			break;
		}

		case GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA:
			return "Not yet implemented for this kind of data types";

		default:
			return "unknown data type";
	}

	payloadField.dataType = dataType;
	memcpy(payloadField.data, data, sizeof(data));
	payloadField.dataLenght = dataLenght;

	return NULL;
}

#endif //BLE_CLIAPP_GAP_ADVERTISING_DATA_SERIALIZER_H_