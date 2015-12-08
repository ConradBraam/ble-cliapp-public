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
