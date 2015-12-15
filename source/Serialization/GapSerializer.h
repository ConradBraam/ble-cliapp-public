#ifndef BLE_CLIAPP_GAP_SERIALIZER_H_
#define BLE_CLIAPP_GAP_SERIALIZER_H_

#include "Serializer.h"
#include "ble/Gap.h"
#include "dynamic/Value.h"

template<>
struct SerializerDescription<Gap::AddressType_t> {
    typedef Gap::AddressType_t type;

    static const ConstArray<ValueToStringMapping<Gap::AddressType_t> > mapping() {
        static const ValueToStringMapping<Gap::AddressType_t> map[] = {
            { BLEProtocol::AddressType::PUBLIC, "ADDR_TYPE_PUBLIC" },
            { BLEProtocol::AddressType::RANDOM_STATIC, "ADDR_TYPE_RANDOM_STATIC" },
            { BLEProtocol::AddressType::RANDOM_PRIVATE_RESOLVABLE, "ADDR_TYPE_RANDOM_PRIVATE_RESOLVABLE" },
            { BLEProtocol::AddressType::RANDOM_PRIVATE_NON_RESOLVABLE, "ADDR_TYPE_RANDOM_PRIVATE_NON_RESOLVABLE" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown Gap::AddressType_t";
    }
};

template<>
struct SerializerDescription<Gap::Role_t> {
    typedef Gap::Role_t type;

    static const ConstArray<ValueToStringMapping<type> > mapping() {
        static const ValueToStringMapping<type> map[] = {
            { Gap::PERIPHERAL, "PERIPHERAL" },
            { Gap::CENTRAL, "CENTRAL" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown Gap::Role_t";
    }
};

template<>
struct SerializerDescription<Gap::DisconnectionReason_t> {
    typedef Gap::DisconnectionReason_t type;

    static const ConstArray<ValueToStringMapping<type> > mapping() {
        static const ValueToStringMapping<type> map[] = {
            { Gap::CONNECTION_TIMEOUT, "CONNECTION_TIMEOUT" },
            { Gap::REMOTE_USER_TERMINATED_CONNECTION, "REMOTE_USER_TERMINATED_CONNECTION" },
            { Gap::REMOTE_DEV_TERMINATION_DUE_TO_LOW_RESOURCES, "REMOTE_DEV_TERMINATION_DUE_TO_LOW_RESOURCES" },
            { Gap::REMOTE_DEV_TERMINATION_DUE_TO_POWER_OFF, "REMOTE_DEV_TERMINATION_DUE_TO_POWER_OFF" },
            { Gap::LOCAL_HOST_TERMINATED_CONNECTION, "LOCAL_HOST_TERMINATED_CONNECTION" },
            { Gap::CONN_INTERVAL_UNACCEPTABLE, "CONN_INTERVAL_UNACCEPTABLE" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown Gap::DisconnectionReason_t";
    }
};

struct MacAddressString_t {
    char str[sizeof("XX:XX:XX:XX:XX:XX")];
};

// MAC address serializer/deserializer
static inline bool macAddressFromString(const char* str, Gap::Address_t& val) {
    // since nano libc does not properly handle all format flags, we rely on integers for sscanf
    int tmp[Gap::ADDR_LEN];

    int count = sscanf(str, "%x:%x:%x:%x:%x:%x", &tmp[5], &tmp[4], &tmp[3], &tmp[2], &tmp[1], &tmp[0]);

    if(count == Gap::ADDR_LEN) {
        for(size_t i = 0; i < Gap::ADDR_LEN; ++i) {
            val[i] = (uint8_t) tmp[i];
        }
        return true;
    }

    return false;
}

static inline MacAddressString_t macAddressToString(const Gap::Address_t& src) {
    MacAddressString_t converted;
    snprintf(converted.str, sizeof(converted.str), "%02X:%02X:%02X:%02X:%02X:%02X", src[5], src[4], src[3],src[2],src[1],src[0]);
    return converted;
}


// TODO : bidirectional way of serialization / deserialization

/// convert Gap::ConnectionParams_t from CLI to structure
static inline bool connectionParamsFromCLI(const char* str, Gap::ConnectionParams_t& params) {
    // since nano libc does not properly handle all format flags, we rely on integers for sscanf
    unsigned int tmp[4];

    int count = sscanf(str, "%u,%u,%u,%u", &tmp[0], &tmp[1], &tmp[2], &tmp[3]);

    if(count == 4) {
        params.minConnectionInterval = tmp[0];
        params.maxConnectionInterval = tmp[1];
        params.slaveLatency = tmp[2];
        params.connectionSupervisionTimeout = tmp[3];
        return true;
    }

    return false;
}

static inline dynamic::Value connectionParamsToJSON(const Gap::ConnectionParams_t& params) {
    dynamic::Value res;
    res["minConnectionInterval"_ss] = (int64_t) params.minConnectionInterval;
    res["maxConnectionInterval"_ss] = (int64_t) params.maxConnectionInterval;
    res["slaveLatency"_ss] = (int64_t) params.slaveLatency;
    res["connectionSupervisionTimeout"_ss] = (int64_t) params.connectionSupervisionTimeout;
    return res;
}

// TODO better than that, this has to be generic !!!!
static inline dynamic::Value txPermittedValuesToJSON(const int8_t* permittedTxPowerValues, size_t permittedTxPowerValuesCount) {
    dynamic::Value result;
    for(size_t i = 0; i < permittedTxPowerValuesCount; ++i) {
        result.push_back((int64_t) permittedTxPowerValues[i]);
    }

    return result;
}

static inline dynamic::Value gapStateToJSON(Gap::GapState_t state) {
    dynamic::Value result;
    result["advertising"_ss] = state.advertising ? true : false;
    result["connected"_ss] = state.connected ? true : false;
    return result;
}

#endif //BLE_CLIAPP_GAP_SERIALIZER_H_
