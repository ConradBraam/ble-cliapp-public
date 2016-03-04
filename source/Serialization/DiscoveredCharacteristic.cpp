#include "DiscoveredCharacteristic.h"
#include "UUID.h"

serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const DiscoveredCharacteristic& characteristic) {
    using namespace serialization;

    return os << startObject <<
        key("UUID") << characteristic.getUUID() <<
        key("properties") << characteristic.getProperties() <<
        key("start_handle") << characteristic.getDeclHandle() <<
        key("value_handle") << characteristic.getValueHandle() <<
        key("end_handle") << characteristic.getLastHandle() <<
    endObject;

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
