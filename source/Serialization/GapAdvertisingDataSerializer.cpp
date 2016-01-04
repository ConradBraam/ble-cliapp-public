#include "Serializer.h"
#include "ble/GapAdvertisingData.h"
#include "ble/UUID.h"
#include <algorithm>
#include "UUID.h"
#include "Hex.h"

#include "GapAdvertisingDataSerializer.h"
#include <cstring>

dynamic::Value gapAdvertisingDataToJSON(const uint8_t* data, uint8_t size) {
    using container::StaticString;
    using container::DynamicString;
    using dynamic::Value;

    Value result;

    for(size_t i = 0; i < size; i = (i + data[i] + 1)) {
        if(data[i] == 0) {
            continue;
        }

        // Setup field access
        GapAdvertisingData::DataType_t dataType = (GapAdvertisingData::DataType_t) data[i + 1];
        const uint8_t* fieldData = (data[i] == 1) ? NULL : data + i + 2;
        uint8_t dataLenght = data[i] - 1;

        Value fieldValue = "conversion not handled"_ss;

        // serialization
        switch(dataType) {
            case GapAdvertisingData::FLAGS: {
                // if the flags are malformed, we signal the error
                if(dataLenght != 1) {
                    // TODO : add the true value of the field
                    fieldValue = "malformed"_ss;
                    break;
                }

                // construct an array of the flags
                const ConstArray<ValueToStringMapping<GapAdvertisingData::Flags_t> > mapping = SerializerDescription<GapAdvertisingData::Flags_t>::mapping();
                GapAdvertisingData::Flags_t flags = (GapAdvertisingData::Flags_t) fieldData[0];

                for(size_t j = 0; j < mapping.count(); ++j) {
                    if(flags & mapping[j].value) {
                        fieldValue.push_back(StaticString(mapping[j].str));
                    }
                }

                break;
            }

            case GapAdvertisingData::INCOMPLETE_LIST_16BIT_SERVICE_IDS:
            case GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS:
                // if the flags are malformed, we signal the error
                if(dataLenght % sizeof(uint16_t)) {
                    // TODO : add the true value of the field
                    fieldValue = "malformed"_ss;
                    break;
                }

                // construct an array of the service ids
                for(size_t j = 0; j < dataLenght; j += sizeof(uint16_t)) {
                    uint16_t uuid = 0;
                    memcpy(&uuid, fieldData + j, sizeof(uuid));
                    char uuidStr[2 + sizeof(uint16_t) * 2 + 1] = { 0 };
                    snprintf(uuidStr, sizeof(uuidStr), "0x%04X", uuid);
                    fieldValue.push_back(DynamicString(uuidStr));
                }
                break;

            case GapAdvertisingData::INCOMPLETE_LIST_32BIT_SERVICE_IDS:
            case GapAdvertisingData::COMPLETE_LIST_32BIT_SERVICE_IDS:
                // if the flags are malformed, we signal the error
                if(dataLenght % sizeof(uint32_t)) {
                    // TODO : add the true value of the field
                    fieldValue = "malformed"_ss;
                    break;
                }

                // construct an array of the service ids
                for(size_t j = 0; j < dataLenght; j += sizeof(uint32_t)) {
                    uint32_t uuid = 0;
                    memcpy(&uuid, fieldData + j, sizeof(uuid));
                    char uuidStr[2 + sizeof(uint32_t) * 2 + 1] = { 0 };
                    snprintf(uuidStr, sizeof(uuidStr), "0x%08lX", uuid);
                    fieldValue.push_back(DynamicString(uuidStr));
                }
                break;

            case GapAdvertisingData::INCOMPLETE_LIST_128BIT_SERVICE_IDS:
            case GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS:
                break;

            case GapAdvertisingData::SHORTENED_LOCAL_NAME:
            case GapAdvertisingData::COMPLETE_LOCAL_NAME:
                if(dataLenght == 0) {
                    // TODO : add the true value of the field
                    fieldValue = ""_ss;
                    break;
                }

                fieldValue = DynamicString((const char*)fieldData, dataLenght);
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

            case GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA: {
                char hexData[(GAP_ADVERTISING_DATA_MAX_PAYLOAD * 2) + 1] = { 0 };
                for(size_t j = 0; j < dataLenght; ++j) {
                    snprintf(hexData + (j * 2), 3, "%02X", fieldData[j]);
                }
                fieldValue = hexData;
            } break;
        }

        result[StaticString(toString(dataType))] = fieldValue;
    }

    char rawData[(GAP_ADVERTISING_DATA_MAX_PAYLOAD * 2) + 1] = { 0 };
    for(size_t i = 0; i < size; ++i) {
        snprintf(rawData + (i * 2), 3, "%02X", data[i]);
    }

    result["raw"_ss] = DynamicString((const char*) rawData);

    return result;
}


// TODO : data to string as hex ...
dynamic::Value gapAdvertisingDataToJSON(const GapAdvertisingData& advertisingData) {
    return gapAdvertisingDataToJSON(advertisingData.getPayload(), advertisingData.getPayloadLen());
}

serialization::JSONOutputStream& serializeGapAdvertisingData(serialization::JSONOutputStream& os, const uint8_t* data, uint8_t size) {
    using namespace serialization;

    os << startObject;

    for(size_t i = 0; i < size; i = (i + data[i] + 1)) {
        if(data[i] == 0) {
            continue;
        }

        // Setup field access
        GapAdvertisingData::DataType_t dataType = (GapAdvertisingData::DataType_t) data[i + 1];
        const uint8_t* fieldData = (data[i] == 1) ? NULL : data + i + 2;
        uint8_t dataLenght = data[i] - 1;

        os << key(toString(dataType));

        // serialization
        switch(dataType) {
            case GapAdvertisingData::FLAGS: {
                // if the flags are malformed, we signal the error
                if(dataLenght != 1) {
                    // TODO : add the true value of the field
                    os << "malformed";
                    break;
                }

                // construct an array of the flags
                const ConstArray<ValueToStringMapping<GapAdvertisingData::Flags_t> > mapping = SerializerDescription<GapAdvertisingData::Flags_t>::mapping();
                GapAdvertisingData::Flags_t flags = (GapAdvertisingData::Flags_t) fieldData[0];

                os << startArray;
                for(size_t j = 0; j < mapping.count(); ++j) {
                    if(flags & mapping[j].value) {
                        os << mapping[j].str;
                    }
                }
                os << endArray;

                break;
            }

            case GapAdvertisingData::INCOMPLETE_LIST_16BIT_SERVICE_IDS:
            case GapAdvertisingData::COMPLETE_LIST_16BIT_SERVICE_IDS:
                // if the flags are malformed, we signal the error
                if(dataLenght % sizeof(uint16_t)) {
                    // TODO : add the true value of the field
                    os << "malformed";
                    break;
                }

                // construct an array of the service ids
                os << startArray;
                for(size_t j = 0; j < dataLenght; j += sizeof(uint16_t)) {
                    uint16_t uuid = 0;
                    memcpy(&uuid, fieldData + j, sizeof(uuid));
                    os.formatValue("\"0x%04X\"", uuid);
                }
                os << endArray;
                break;

            case GapAdvertisingData::INCOMPLETE_LIST_32BIT_SERVICE_IDS:
            case GapAdvertisingData::COMPLETE_LIST_32BIT_SERVICE_IDS:
                // if the flags are malformed, we signal the error
                if(dataLenght % sizeof(uint32_t)) {
                    // TODO : add the true value of the field
                    os << "malformed";
                    break;
                }

                // construct an array of the service ids
                os << startArray;
                for(size_t j = 0; j < dataLenght; j += sizeof(uint32_t)) {
                    uint32_t uuid = 0;
                    memcpy(&uuid, fieldData + j, sizeof(uuid));
                    os.format("\"0x%08lX\"", uuid);
                }
                os << endArray;
                break;

            case GapAdvertisingData::INCOMPLETE_LIST_128BIT_SERVICE_IDS:
            case GapAdvertisingData::COMPLETE_LIST_128BIT_SERVICE_IDS:
                os << "";
                break;

            case GapAdvertisingData::SHORTENED_LOCAL_NAME:
            case GapAdvertisingData::COMPLETE_LOCAL_NAME:
                if(dataLenght == 0) {
                    // TODO : add the true value of the field
                    os << "";
                    break;
                }

                os.put('"');
                os.write((const char*)fieldData, dataLenght);
                os.put('"');
                os.commitValue();
                break;

            case GapAdvertisingData::TX_POWER_LEVEL:
                os << "";
                break;

            case GapAdvertisingData::DEVICE_ID:
                os << "";
                break;

            case GapAdvertisingData::SLAVE_CONNECTION_INTERVAL_RANGE:
                os << "";
                break;

            case GapAdvertisingData::SERVICE_DATA:
                os << "";
                break;

            case GapAdvertisingData::APPEARANCE:
                os << "";
                break;

            case GapAdvertisingData::ADVERTISING_INTERVAL:
                os << "";
                break;

            case GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA: {
                os.put('"');
                for(size_t j = 0; j < dataLenght; ++j) {
                    os.format("%02X", fieldData[j]);
                }
                os.put('"');
                os.commitValue();
            } break;
        }
    }

    os << key("raw");
    os.put('"');
    for(size_t i = 0; i < size; ++i) {
        os.format("%02X", data[i]);
    }
    os.put('"');
    os.commitValue();

    os << endObject;

    return os;
}

serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const GapAdvertisingData& advertisingData) {
    return serializeGapAdvertisingData(os, advertisingData.getPayload(), advertisingData.getPayloadLen());
}



/**
 * return NULL in case of success or the reason of the error
 */
const char* advertisingPayloadFieldFromCLI(const CommandArgs& args, AdvertisingPayloadField_t& payloadField) {
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

            UUID uuid;
            if(!fromString(args[1], uuid)) {
                return "The uuid is not properly formated (XXXXXXX-XXXX-XXXX-XXXX-XXXXXXXXXXXX)";
            }

            memcpy(data, uuid.getBaseUUID(), UUID::LENGTH_OF_LONG_UUID);
            dataLenght = UUID::LENGTH_OF_LONG_UUID;
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

        case GapAdvertisingData::MANUFACTURER_SPECIFIC_DATA: {
            if(args.count() != 2) {
                return "too many arguments, only the manufacturer specific data is expected";
            }

            size_t len = strlen(args[1]);

            if (len % 2) {
                return "manufacturer data should be an hex string";
            }

            if (len > (sizeof(data) * 2)) {
                return "manufacturer data provided are too long";
            }

            for(size_t i = 0; i < len / 2; ++i) {
                if(asciiHexByteToByte(args[1][i * 2], args[1][(i * 2) + 1], data[i]) == false) {
                    return "invalid hex data";
                }
            }

            dataLenght = len / 2;
            break;
        }

        default:
            return "unknown data type";
    }

    payloadField.dataType = dataType;
    memcpy(payloadField.data, data, sizeof(data));
    payloadField.dataLenght = dataLenght;

    return NULL;
}
