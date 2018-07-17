#ifndef BLE_CLIAPP_GAP_SERIALIZER_H_
#define BLE_CLIAPP_GAP_SERIALIZER_H_

#include "Serialization/Serializer.h"
#include "ble/Gap.h"
#include "Serialization/JSONOutputStream.h"

template<>
struct SerializerDescription<ble::phy_t> {
    typedef ble::phy_t type;

    static const ConstArray<ValueToStringMapping<type> > mapping() {
        static const ValueToStringMapping<type> map[] = {
            { ble::phy_t::LE_1M, "LE_1M" },
            { ble::phy_t::LE_2M, "LE_2M" },
            { ble::phy_t::LE_CODED, "LE_CODED" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown phy_t";
    }
};

static inline serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, ble::phy_t type) {
    return os << toString(type);
}

template<>
struct SerializerDescription<BLEProtocol::AddressType_t> {
    typedef BLEProtocol::AddressType_t type;

    static const ConstArray<ValueToStringMapping<BLEProtocol::AddressType_t> > mapping() {
        static const ValueToStringMapping<BLEProtocol::AddressType_t> map[] = {
            { BLEProtocol::AddressType::PUBLIC, "ADDR_TYPE_PUBLIC" },
            { BLEProtocol::AddressType::RANDOM_STATIC, "ADDR_TYPE_RANDOM_STATIC" },
            { BLEProtocol::AddressType::RANDOM_PRIVATE_RESOLVABLE, "ADDR_TYPE_RANDOM_PRIVATE_RESOLVABLE" },
            { BLEProtocol::AddressType::RANDOM_PRIVATE_NON_RESOLVABLE, "ADDR_TYPE_RANDOM_PRIVATE_NON_RESOLVABLE" },
            { BLEProtocol::AddressType::PUBLIC, "PUBLIC" },
            { BLEProtocol::AddressType::RANDOM_STATIC, "RANDOM_STATIC" },
            { BLEProtocol::AddressType::RANDOM_PRIVATE_RESOLVABLE, "RANDOM_PRIVATE_RESOLVABLE" },
            { BLEProtocol::AddressType::RANDOM_PRIVATE_NON_RESOLVABLE, "RANDOM_PRIVATE_NON_RESOLVABLE" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown BLEProtocol::AddressType_t";
    }
};

static inline serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, BLEProtocol::AddressType_t addr) {
    return os << toString(addr);
}

template<>
struct SerializerDescription<ble::peer_address_type_t> {
    typedef ble::peer_address_type_t type;

    static const ConstArray<ValueToStringMapping<ble::peer_address_type_t> > mapping() {
        static const ValueToStringMapping<ble::peer_address_type_t> map[] = {
            { ble::peer_address_type_t::PUBLIC, "PUBLIC" },
            { ble::peer_address_type_t::RANDOM, "RANDOM" },
            { ble::peer_address_type_t::PUBLIC_IDENTITY, "PUBLIC_IDENTITY" },
            { ble::peer_address_type_t::RANDOM_STATIC_IDENTITY, "RANDOM_STATIC_IDENTITY" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown ble::peer_address_type_t";
    }
};

static inline serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, ble::peer_address_type_t addr) {
    return os << toString(addr);
}


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

static inline serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, Gap::DisconnectionReason_t reason) {
    return os << toString(reason);
}

struct MacAddressString_t {
    char str[sizeof("XX:XX:XX:XX:XX:XX")];
};

// MAC address serializer/deserializer
bool macAddressFromString(const char* str, Gap::Address_t& val);
MacAddressString_t macAddressToString(const Gap::Address_t& src);

struct MacAddress_t { 
    Gap::Address_t value;

    operator Gap::Address_t&() { 
        return value;
    }

    operator const Gap::Address_t&() const { 
        return value;
    }
};

static inline bool fromString(const char* str, MacAddress_t& val) { 
    return macAddressFromString(str, val);
}


serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const Gap::Address_t& addr);

/// convert Gap::ConnectionParams_t from CLI to structure
bool connectionParamsFromCLI(const char* str, Gap::ConnectionParams_t& params);


static inline bool fromString(const char* str, Gap::ConnectionParams_t& params) { 
    return connectionParamsFromCLI(str, params);
}


serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os,const Gap::ConnectionParams_t&);

serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const Gap::GapState_t& state);

/**
 * @brief serializationn and deserialization of Gap::AdvertisingPolicyMode_t
 */
template<>
struct SerializerDescription<Gap::AdvertisingPolicyMode_t> {
    typedef Gap::AdvertisingPolicyMode_t type;

    static const ConstArray<ValueToStringMapping<type> > mapping() {
        static const ValueToStringMapping<type> map[] = {
            { Gap::ADV_POLICY_IGNORE_WHITELIST, "ADV_POLICY_IGNORE_WHITELIST" },
            { Gap::ADV_POLICY_FILTER_SCAN_REQS, "ADV_POLICY_FILTER_SCAN_REQS" },
            { Gap::ADV_POLICY_FILTER_CONN_REQS, "ADV_POLICY_FILTER_CONN_REQS" },
            { Gap::ADV_POLICY_FILTER_ALL_REQS, "ADV_POLICY_FILTER_ALL_REQS" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown Gap::AdvertisingPolicyMode_t";
    }
};

static inline serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, Gap::AdvertisingPolicyMode_t reason) {
    return os << toString(reason);
}

/**
 * @brief serialization and deserialization of Gap::ScanningPolicyMode_t
 */
template<>
struct SerializerDescription<Gap::ScanningPolicyMode_t> {
    typedef Gap::ScanningPolicyMode_t type;

    static const ConstArray<ValueToStringMapping<type> > mapping() {
        static const ValueToStringMapping<type> map[] = {
            { Gap::SCAN_POLICY_IGNORE_WHITELIST, "SCAN_POLICY_IGNORE_WHITELIST" },
            { Gap::SCAN_POLICY_FILTER_ALL_ADV, "SCAN_POLICY_FILTER_ALL_ADV" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown Gap::ScanningPolicyMode_t";
    }
};

static inline serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, Gap::ScanningPolicyMode_t reason) {
    return os << toString(reason);
}

/**
 * @brief serialization and deserialization of Gap::InitiatorPolicyMode_t
 */
template<>
struct SerializerDescription<Gap::InitiatorPolicyMode_t> {
    typedef Gap::InitiatorPolicyMode_t type;

    static const ConstArray<ValueToStringMapping<type> > mapping() {
        static const ValueToStringMapping<type> map[] = {
            { Gap::INIT_POLICY_IGNORE_WHITELIST, "INIT_POLICY_IGNORE_WHITELIST" },
            { Gap::INIT_POLICY_FILTER_ALL_ADV, "INIT_POLICY_FILTER_ALL_ADV" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown Gap::InitiatorPolicyMode_t";
    }
};

static inline serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, Gap::InitiatorPolicyMode_t reason) {
    return os << toString(reason);
}

/**
 * @brief serialization and deserialization of Gap::PeripheralPrivacyConfiguration_t::ResolutionStrategy
 */
template<>
struct SerializerDescription<Gap::PeripheralPrivacyConfiguration_t::ResolutionStrategy> {
    typedef Gap::PeripheralPrivacyConfiguration_t::ResolutionStrategy type;

    static const ConstArray<ValueToStringMapping<type> > mapping() {
        static const ValueToStringMapping<type> map[] = {
            { Gap::PeripheralPrivacyConfiguration_t::DO_NOT_RESOLVE, "DO_NOT_RESOLVE" },
            { Gap::PeripheralPrivacyConfiguration_t::REJECT_NON_RESOLVED_ADDRESS, "REJECT_NON_RESOLVED_ADDRESS" },
            { Gap::PeripheralPrivacyConfiguration_t::PERFORM_PAIRING_PROCEDURE, "PERFORM_PAIRING_PROCEDURE" },
            { Gap::PeripheralPrivacyConfiguration_t::PERFORM_AUTHENTICATION_PROCEDURE, "PERFORM_AUTHENTICATION_PROCEDURE" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown Gap::PeripheralPrivacyConfiguration_t::ResolutionStrategy";
    }
};

static inline serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, Gap::PeripheralPrivacyConfiguration_t::ResolutionStrategy strategy) {
    return os << toString(strategy);
}

static inline serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const Gap::PeripheralPrivacyConfiguration_t& configuration) {
    using namespace serialization;
    return os << startObject <<
        key("use_non_resolvable_random_address") << configuration.use_non_resolvable_random_address <<
        key("resolution_strategy") << configuration.resolution_strategy <<
    endObject;
}

/**
 * @brief serialization and deserialization of Gap::CentralPrivacyConfiguration_t::ResolutionStrategy
 */
template<>
struct SerializerDescription<Gap::CentralPrivacyConfiguration_t::ResolutionStrategy> {
    typedef Gap::CentralPrivacyConfiguration_t::ResolutionStrategy type;

    static const ConstArray<ValueToStringMapping<type> > mapping() {
        static const ValueToStringMapping<type> map[] = {
            { Gap::CentralPrivacyConfiguration_t::DO_NOT_RESOLVE, "DO_NOT_RESOLVE" },
            { Gap::CentralPrivacyConfiguration_t::RESOLVE_AND_FILTER, "RESOLVE_AND_FILTER" },
            { Gap::CentralPrivacyConfiguration_t::RESOLVE_AND_FORWARD, "RESOLVE_AND_FORWARD" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown Gap::CentralPrivacyConfiguration_t::ResolutionStrategy";
    }
};

static inline serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, Gap::CentralPrivacyConfiguration_t::ResolutionStrategy strategy) {
    return os << toString(strategy);
}

static inline serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const Gap::CentralPrivacyConfiguration_t& configuration) {
    using namespace serialization;
    return os << startObject <<
        key("use_non_resolvable_random_address") << configuration.use_non_resolvable_random_address <<
        key("resolution_strategy") << configuration.resolution_strategy <<
    endObject;
}

#endif //BLE_CLIAPP_GAP_SERIALIZER_H_
