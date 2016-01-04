#include "DiscoveredCharacteristic.h"
#include "UUID.h"

dynamic::Value toDynamicValue(const DiscoveredCharacteristic* discoveredCharacteristic) {
    dynamic::Value characteristic;

    characteristic["UUID"_ss] = toString(discoveredCharacteristic->getUUID());
    characteristic["properties"_ss] = toDynamicValue(discoveredCharacteristic->getProperties());
    characteristic["start_handle"_ss] = (int64_t) discoveredCharacteristic->getDeclHandle();
    characteristic["value_handle"_ss] = (int64_t) discoveredCharacteristic->getValueHandle();
    characteristic["end_handle"_ss] = "not yet implemented, require support from BLE API"_ss;

    return characteristic;
}

serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const DiscoveredCharacteristic& characteristic) {
    using namespace serialization;

    return os << startObject <<
        key("UUID") << characteristic.getUUID() <<
        key("properties") << characteristic.getProperties() <<
        key("start_handle") << characteristic.getDeclHandle() <<
        key("value_handle") << characteristic.getValueHandle() <<
        key("end_handle") << "not yet implemented, require support from BLE API" <<
    endObject;

}

dynamic::Value toDynamicValue(const DiscoveredCharacteristic::Properties_t& properties) {
    dynamic::Value result;

    if (properties.broadcast()) {
        result.push_back("broadcast"_ss);
    }
    if (properties.read()) {
        result.push_back("read"_ss);
    }
    if (properties.writeWoResp()) {
        result.push_back("writeWoResp"_ss);
    }
    if (properties.write()) {
        result.push_back("write"_ss);
    }
    if (properties.notify()) {
        result.push_back("notify"_ss);
    }
    if (properties.indicate()) {
        result.push_back("indicate"_ss);
    }
    if (properties.authSignedWrite()) {
        result.push_back("authSignedWrite"_ss);
    }

    return result;
}


serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const DiscoveredCharacteristic::Properties_t& properties) {
    using namespace serialization;

    os << startArray;

    if (properties.broadcast()) {
        os << "broadcast";
    }
    if (properties.read()) {
        os << "read";
    }
    if (properties.writeWoResp()) {
        os << "writeWoResp";
    }
    if (properties.write()) {
        os << "write";
    }
    if (properties.notify()) {
        os << "notify";
    }
    if (properties.indicate()) {
        os << "indicate";
    }
    if (properties.authSignedWrite()) {
        os << "authSignedWrite";
    }

    return os << endArray;
}
