#ifndef BLE_CLIAPP_GAP_SERIALIZER_H_
#define BLE_CLIAPP_GAP_SERIALIZER_H_

#include <stdio.h>
#include "ble/Gap.h"
#include "Serialization/Serializer.h"
#include "Serialization/JSONOutputStream.h"

template<>
struct SerializerDescription<ble::phy_t::type> {
    typedef ble::phy_t::type type;

    static const ConstArray<ValueToStringMapping<type> > mapping() {
        static const ValueToStringMapping<type> map[] = {
            { ble::phy_t::LE_1M, "LE_1M" },
            { ble::phy_t::LE_2M, "LE_2M" },
            { ble::phy_t::LE_CODED, "LE_CODED" },
            { ble::phy_t::NONE, "NONE" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown phy_t";
    }
};

static inline serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, ble::phy_t type) {
    return os << toString((ble::phy_t::type) type.value());
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
            { ble::peer_address_type_t::RANDOM_STATIC_IDENTITY, "RANDOM_STATIC_IDENTITY" },
            { ble::peer_address_type_t::ANONYMOUS, "ANONYMOUS" }
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

static inline bool fromString(const char* str, ble::address_t& val) {
    return macAddressFromString(str, reinterpret_cast<Gap::Address_t&>(*val.data()));
}

serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const Gap::Address_t& addr);

inline serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const ble::address_t& addr) {
    MacAddressString_t converted;
    snprintf(converted.str, sizeof(converted.str), "%02X:%02X:%02X:%02X:%02X:%02X",
        addr[5], addr[4], addr[3],addr[2],addr[1],addr[0]);
    return os << converted.str;
}

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
 * @brief serialization and deserialization of Gap::PeripheralPrivacyConfiguration_t::resolution_strategy_t
 */
template<>
struct SerializerDescription<Gap::PeripheralPrivacyConfiguration_t::resolution_strategy_t> {
    typedef Gap::PeripheralPrivacyConfiguration_t::resolution_strategy_t type;

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
        return "unknown Gap::PeripheralPrivacyConfiguration_t::resolution_strategy_t";
    }
};

static inline serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, Gap::PeripheralPrivacyConfiguration_t::resolution_strategy_t strategy) {
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
 * @brief serialization and deserialization of Gap::CentralPrivacyConfiguration_t::resolution_strategy_t
 */
template<>
struct SerializerDescription<Gap::CentralPrivacyConfiguration_t::resolution_strategy_t> {
    typedef Gap::CentralPrivacyConfiguration_t::resolution_strategy_t type;

    static const ConstArray<ValueToStringMapping<type> > mapping() {
        static const ValueToStringMapping<type> map[] = {
            { Gap::CentralPrivacyConfiguration_t::DO_NOT_RESOLVE, "DO_NOT_RESOLVE" },
            { Gap::CentralPrivacyConfiguration_t::RESOLVE_AND_FILTER, "RESOLVE_AND_FILTER" },
            { Gap::CentralPrivacyConfiguration_t::RESOLVE_AND_FORWARD, "RESOLVE_AND_FORWARD" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown Gap::CentralPrivacyConfiguration_t::resolution_strategy_t";
    }
};

static inline serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, Gap::CentralPrivacyConfiguration_t::resolution_strategy_t strategy) {
    return os << toString(strategy);
}

static inline serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, const Gap::CentralPrivacyConfiguration_t& configuration) {
    using namespace serialization;
    return os << startObject <<
        key("use_non_resolvable_random_address") << configuration.use_non_resolvable_random_address <<
        key("resolution_strategy") << configuration.resolution_strategy <<
    endObject;
}

template<>
struct SerializerDescription<ble::advertising_type_t::type> {
    typedef ble::advertising_type_t::type type;

    static const ConstArray<ValueToStringMapping<type> > mapping() {
        static const ValueToStringMapping<type> map[] = {
            { ble::advertising_type_t::CONNECTABLE_UNDIRECTED, "CONNECTABLE_UNDIRECTED" },
            { ble::advertising_type_t::CONNECTABLE_DIRECTED, "CONNECTABLE_DIRECTED" },
            { ble::advertising_type_t::SCANNABLE_UNDIRECTED, "SCANNABLE_UNDIRECTED" },
            { ble::advertising_type_t::NON_CONNECTABLE_UNDIRECTED, "NON_CONNECTABLE_UNDIRECTED" },
            { ble::advertising_type_t::CONNECTABLE_DIRECTED_LOW_DUTY, "CONNECTABLE_DIRECTED_LOW_DUTY" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown ble::advertising_type_t";
    }
};

template<typename Layout, uint32_t TB, typename R, typename F>
bool fromString(const char* str, ble::Duration<Layout, TB, R, F>& duration) {
    Layout v = 0;
    if (!fromString(str, v)) {
        return false;
    }

    if (v < duration.MIN || v > duration.MAX) {
        return false;
    }

    duration = ble::Duration<Layout, TB, R, F>(v);
    return true;
}

template<typename Layout, uint32_t TB, typename R, typename F>
serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, ble::Duration<Layout, TB, R, F> duration) {
    return os << duration.value();
}

template<>
struct SerializerDescription<ble::own_address_type_t::type > {
    typedef ble::own_address_type_t::type type;

    static const ConstArray<ValueToStringMapping<type> > mapping() {
        static const ValueToStringMapping<type> map[] = {
            { ble::own_address_type_t::PUBLIC, "PUBLIC" },
            { ble::own_address_type_t::RANDOM, "RANDOM" },
            { ble::own_address_type_t::RESOLVABLE_PRIVATE_ADDRESS_PUBLIC_FALLBACK, "RESOLVABLE_PRIVATE_ADDRESS_PUBLIC_FALLBACK" },
            { ble::own_address_type_t::RESOLVABLE_PRIVATE_ADDRESS_RANDOM_FALLBACK, "RESOLVABLE_PRIVATE_ADDRESS_RANDOM_FALLBACK" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown ble::own_address_type_t";
    }
};

template<>
struct SerializerDescription<ble::advertising_filter_policy_t::type > {
    typedef ble::advertising_filter_policy_t::type type;

    static const ConstArray<ValueToStringMapping<type> > mapping() {
        static const ValueToStringMapping<type> map[] = {
            { ble::advertising_filter_policy_t::NO_FILTER, "NO_FILTER" },
            { ble::advertising_filter_policy_t::FILTER_SCAN_REQUESTS, "FILTER_SCAN_REQUESTS" },
            { ble::advertising_filter_policy_t::FILTER_CONNECTION_REQUEST, "FILTER_CONNECTION_REQUEST" },
            { ble::advertising_filter_policy_t::FILTER_SCAN_AND_CONNECTION_REQUESTS, "FILTER_SCAN_AND_CONNECTION_REQUESTS" },
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown ble::advertising_filter_policy_t";
    }
};

template<>
struct SerializerDescription<ble::scanning_filter_policy_t::type > {
    typedef ble::scanning_filter_policy_t::type type;

    static const ConstArray<ValueToStringMapping<type> > mapping() {
        static const ValueToStringMapping<type> map[] = {
            { ble::scanning_filter_policy_t::NO_FILTER, "NO_FILTER" },
            { ble::scanning_filter_policy_t::FILTER_ADVERTISING, "FILTER_ADVERTISING" },
            { ble::scanning_filter_policy_t::NO_FILTER_INCLUDE_UNRESOLVABLE_DIRECTED, "NO_FILTER_INCLUDE_UNRESOLVABLE_DIRECTED" },
            { ble::scanning_filter_policy_t::FILTER_ADVERTISING_INCLUDE_UNRESOLVABLE_DIRECTED, "FILTER_ADVERTISING_INCLUDE_UNRESOLVABLE_DIRECTED" },
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown ble::scanning_filter_policy_t";
    }
};


template<>
struct SerializerDescription<ble::initiator_filter_policy_t::type> {
    typedef ble::initiator_filter_policy_t::type type;

    static const ConstArray<ValueToStringMapping<type> > mapping() {
        static const ValueToStringMapping<type> map[] = {
            { ble::initiator_filter_policy_t::NO_FILTER, "NO_FILTER" },
            { ble::initiator_filter_policy_t::USE_WHITE_LIST, "USE_WHITE_LIST" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown ble::initiator_filter_policy_t";
    }
};

template<>
struct SerializerDescription<ble::duplicates_filter_t::type> {
    typedef ble::duplicates_filter_t::type type;

    static const ConstArray<ValueToStringMapping<type> > mapping() {
        static const ValueToStringMapping<type> map[] = {
            { ble::duplicates_filter_t::DISABLE, "DISABLE" },
            { ble::duplicates_filter_t::ENABLE, "ENABLE" },
            { ble::duplicates_filter_t::PERIODIC_RESET, "PERIODIC_RESET" },
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown ble::duplicates_filter_t";
    }
};

template<>
struct SerializerDescription<ble::peer_address_type_t::type> {
    typedef ble::peer_address_type_t::type type;

    static const ConstArray<ValueToStringMapping<type> > mapping() {
        static const ValueToStringMapping<type> map[] = {
            { ble::peer_address_type_t::PUBLIC, "PUBLIC" },
            { ble::peer_address_type_t::RANDOM, "RANDOM" },
            { ble::peer_address_type_t::PUBLIC_IDENTITY, "PUBLIC_IDENTITY" },
            { ble::peer_address_type_t::RANDOM_STATIC_IDENTITY, "RANDOM_STATIC_IDENTITY" },
            { ble::peer_address_type_t::ANONYMOUS, "ANONYMOUS" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown ble::peer_address_type_t";
    }
};

template<>
struct SerializerDescription<ble::local_disconnection_reason_t::type> {
    typedef ble::local_disconnection_reason_t::type type;

    static const ConstArray<ValueToStringMapping<type> > mapping() {
        static const ValueToStringMapping<type> map[] = {
            { ble::local_disconnection_reason_t::USER_TERMINATION, "USER_TERMINATION" },
            { ble::local_disconnection_reason_t::AUTHENTICATION_FAILURE, "AUTHENTICATION_FAILURE" },
            { ble::local_disconnection_reason_t::LOW_RESOURCES, "LOW_RESOURCES" },
            { ble::local_disconnection_reason_t::PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED, "PAIRING_WITH_UNIT_KEY_NOT_SUPPORTED" },
            { ble::local_disconnection_reason_t::POWER_OFF, "POWER_OFF" },
            { ble::local_disconnection_reason_t::UNACCEPTABLE_CONNECTION_PARAMETERS, "UNACCEPTABLE_CONNECTION_PARAMETERS" },
            { ble::local_disconnection_reason_t::UNSUPPORTED_REMOTE_FEATURE, "UNSUPPORTED_REMOTE_FEATURE" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown ble::local_disconnection_reason_t";
    }
};

template<>
struct SerializerDescription<ble::controller_supported_features_t::type> {
    typedef ble::controller_supported_features_t::type type;

    static const ConstArray<ValueToStringMapping<type> > mapping() {
        static const ValueToStringMapping<type> map[] = {
            { ble::controller_supported_features_t::LE_EXTENDED_ADVERTISING, "LE_EXTENDED_ADVERTISING" },
            { ble::controller_supported_features_t::LE_2M_PHY, "LE_2M_PHY" },
            { ble::controller_supported_features_t::CHANNEL_SELECTION_ALGORITHM_2, "CHANNEL_SELECTION_ALGORITHM_2" },
            { ble::controller_supported_features_t::CONNECTION_PARAMETERS_REQUEST_PROCEDURE, "CONNECTION_PARAMETERS_REQUEST_PROCEDURE" },
            { ble::controller_supported_features_t::EXTENDED_REJECT_INDICATION, "EXTENDED_REJECT_INDICATION" },
            { ble::controller_supported_features_t::EXTENDED_SCANNER_FILTER_POLICIES, "EXTENDED_SCANNER_FILTER_POLICIES" },
            { ble::controller_supported_features_t::LE_CODED_PHY, "LE_CODED_PHY" },
            { ble::controller_supported_features_t::LE_DATA_PACKET_LENGTH_EXTENSION, "LE_DATA_PACKET_LENGTH_EXTENSION" },
            { ble::controller_supported_features_t::LE_ENCRYPTION, "LE_ENCRYPTION" },
            { ble::controller_supported_features_t::LE_PERIODIC_ADVERTISING, "LE_PERIODIC_ADVERTISING" },
            { ble::controller_supported_features_t::LE_PING, "LE_PING" },
            { ble::controller_supported_features_t::LE_POWER_CLASS, "LE_POWER_CLASS" },
            { ble::controller_supported_features_t::LL_PRIVACY, "LL_PRIVACY" },
            { ble::controller_supported_features_t::SLAVE_INITIATED_FEATURES_EXCHANGE, "SLAVE_INITIATED_FEATURES_EXCHANGE" },
            { ble::controller_supported_features_t::STABLE_MODULATION_INDEX_RECEIVER, "STABLE_MODULATION_INDEX_RECEIVER" },
            { ble::controller_supported_features_t::STABLE_MODULATION_INDEX_TRANSMITTER, "STABLE_MODULATION_INDEX_TRANSMITTER" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown ble::controller_supported_features_t";
    }
};


template<>
struct SerializerDescription<ble::advertising_data_status_t::type> {
    typedef ble::advertising_data_status_t::type type;

    static const ConstArray<ValueToStringMapping<type> > mapping() {
        static const ValueToStringMapping<type> map[] = {
            { ble::advertising_data_status_t::COMPLETE, "COMPLETE" },
            { ble::advertising_data_status_t::INCOMPLETE_MORE_DATA, "INCOMPLETE_MORE_DATA" },
            { ble::advertising_data_status_t::INCOMPLETE_DATA_TRUNCATED, "INCOMPLETE_DATA_TRUNCATED" }
        };

        return makeConstArray(map);
    }

    static const char* errorMessage() {
        return "unknown ble::advertising_data_status_t";
    }
};

inline serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, ble::advertising_data_status_t v) {
    return os << toString((ble::advertising_data_status_t::type) v.value());
}

inline serialization::JSONOutputStream& operator<<(serialization::JSONOutputStream& os, ble::disconnection_reason_t v)
{
    switch (v.value()) {
        case ble::disconnection_reason_t::AUTHENTICATION_FAILURE:
            return os << "AUTHENTICATION_FAILURE";
        case ble::disconnection_reason_t::CONNECTION_TIMEOUT:
            return os << "CONNECTION_TIMEOUT";
        case ble::disconnection_reason_t::REMOTE_USER_TERMINATED_CONNECTION:
            return os << "REMOTE_USER_TERMINATED_CONNECTION";
        case ble::disconnection_reason_t::REMOTE_DEV_TERMINATION_DUE_TO_LOW_RESOURCES:
            return os << "REMOTE_DEV_TERMINATION_DUE_TO_LOW_RESOURCES";
        case ble::disconnection_reason_t::REMOTE_DEV_TERMINATION_DUE_TO_POWER_OFF:
            return os << "REMOTE_DEV_TERMINATION_DUE_TO_POWER_OFF";
        case ble::disconnection_reason_t::LOCAL_HOST_TERMINATED_CONNECTION:
            return os << "LOCAL_HOST_TERMINATED_CONNECTION";
        case ble::disconnection_reason_t::UNACCEPTABLE_CONNECTION_PARAMETERS:
            return os << "UNACCEPTABLE_CONNECTION_PARAMETERS";
        default:
            return os << "unknown disconnection_reason_t";
    }
}

#endif //BLE_CLIAPP_GAP_SERIALIZER_H_
