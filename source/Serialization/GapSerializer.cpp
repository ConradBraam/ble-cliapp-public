#include <stdio.h>
#include "GapSerializer.h"

// MAC address serializer/deserializer
bool macAddressFromString(const char* str, Gap::Address_t& val) {
    // since nano libc does not properly handle all format flags, we rely on integers for sscanf
    int tmp[Gap::ADDR_LEN];

    int count = std::sscanf(str, "%x:%x:%x:%x:%x:%x", &tmp[5], &tmp[4], &tmp[3], &tmp[2], &tmp[1], &tmp[0]);

    if(count == Gap::ADDR_LEN) {
        for(size_t i = 0; i < Gap::ADDR_LEN; ++i) {
            val[i] = (uint8_t) tmp[i];
        }
        return true;
    }

    return false;
}

MacAddressString_t macAddressToString(const Gap::Address_t& src) {
    MacAddressString_t converted;
    snprintf(converted.str, sizeof(converted.str), "%02X:%02X:%02X:%02X:%02X:%02X", src[5], src[4], src[3],src[2],src[1],src[0]);
    return converted;
}

serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const Gap::Address_t& addr) {
    return os.formatValue("\"%02X:%02X:%02X:%02X:%02X:%02X\"", addr[5], addr[4], addr[3],addr[2],addr[1],addr[0]);
}

// TODO : bidirectional way of serialization / deserialization

/// convert Gap::ConnectionParams_t from CLI to structure
bool connectionParamsFromCLI(const char* str, Gap::ConnectionParams_t& params) {
    // since nano libc does not properly handle all format flags, we rely on integers for sscanf
    unsigned int tmp[4];

    int count = std::sscanf(str, "%u,%u,%u,%u", &tmp[0], &tmp[1], &tmp[2], &tmp[3]);

    if(count == 4) {
        params.minConnectionInterval = tmp[0];
        params.maxConnectionInterval = tmp[1];
        params.slaveLatency = tmp[2];
        params.connectionSupervisionTimeout = tmp[3];
        return true;
    }

    return false;
}

serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os,const Gap::ConnectionParams_t& params) {
    using namespace serialization;
    return os << startObject <<
        key("minConnectionInterval") << params.minConnectionInterval <<
        key("maxConnectionInterval") << params.maxConnectionInterval <<
        key("slaveLatency") << params.slaveLatency <<
        key("connectionSupervisionTimeout") << params.connectionSupervisionTimeout <<
    endObject;
}

serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const Gap::GapState_t& state) {
    using namespace serialization;
    return os << startObject <<
        key("advertising") << (state.advertising ? true : false) <<
        key("connected") << (state.connected ? true : false) <<
    endObject;
}
