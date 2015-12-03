#include "GattClientCommands.h"
#include "util/StaticLambda.h"
#include "ble/BLE.h"
#include "Serialization/Serializer.h"

// TODO: description of returned results


// isolation
namespace {
static BLE& ble() {
    return BLE::Instance();
}

static constexpr const Command discoverAllServicesAndCharacteristics {
    "discoverAllServicesAndCharacteristics",
    "discover all services and characteristics available on a peer device",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command discoverAllServices {
    "discoverAllServices",
    "discover all services available on a peer device",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command discoverPrimaryServicesByUUID {
    "discoverPrimaryServicesByUUID",
    "discover a specific kind of services on a peer device",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<serviceUUID>", "The UUID of the services to discover" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command findIncludedServices {
    "findIncludedServices",
    "Find includded services declaration withn a service definition on the server",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<serviceStartHandle>", "The starting handle of the service" },
        { "<serviceEndHandle>", "The ending handle of the service" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command discoverCharacteristicsOfService {
    "discoverCharacteristicsOfService",
    "Discover all characteristics of a service, this procedure will find all the"
    "characteristics declaration within a service definition on a server",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<serviceStartHandle>", "The starting handle of the service" },
        { "<serviceEndHandle>", "The ending handle of the service" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command discoverCharacteristicsByUUID {
    "discoverCharacteristicsByUUID",
    "Discover all characteristics of a service matching a specific UUID.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<serviceStartHandle>", "The starting handle of the service" },
        { "<serviceEndHandle>", "The ending handle of the service" },
        { "<serviceUUID>", "The UUID of the characteristics to discover" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command discoverAllCharacteristicsDescriptors {
    "discoverAllCharacteristicsDescriptors",
    "Find all the characteristic descriptor’s Attribute Handles and Attribute "
    "Types within a characteristic definition. The characteristic specified is "
    "identified by the characteristic handle range.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicValuehandle + 1 >",
          "The starting handle of the descriptors for this characteristic" },
        { "<endHandle>", "The ending handle of the characteristic definition" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command readCharacteristicValue {
    "readCharacteristicValue",
    "Read a characteristic value from a GATT Server using a characteristic value handle.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicValuehandle>", "The handle of characteristic value" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command readUsingCharacteristicUUID {
    "readUsingCharacteristicUUID",
    "This sub-procedure is used to read a Characteristic Value from a server "
    "when the client only knows the characteristic UUID and does not know the "
    "handle of the characteristic.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<serviceStartHandle>", "The starting handle of the service" },
        { "<serviceEndHandle>", "The ending handle of the service" },
        { "<characteristicUUID>", "The UUID of the characteristic" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command readLongCharacteristicValue {
    "readLongCharacteristicValue",
    "Read a characteristic value from a server when the client knows the "
    "characteristic value handle and the length of the characteristic value "
    "is longer than can be sent in a single read response",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicValuehandle>", "The handle of characteristic value" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command readMultipleCharacteristicValues {
    "readMultipleCharacteristicValues",
    "Read a multiple characteristics values from a set of characteristics value "
    "handle.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicValuehandles...>", "Handles of characteristics values to read" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command writeWithoutResponse {
    "writeWithoutResponse",
    "Write a characteristic value to a server, the server will not acknowledge anything.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicValuehandle>", "Handle of the characteristic value to write" },
        { "<value>", "Hexadecimal string representation of the value to write" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command signedWriteWithoutResponse {
    "signedWriteWithoutResponse",
    "Write a characteristic value to a server, the server will not acknowledge anything. "
    "This sub-procedure shall only be used if the CharacteristicProperties authenticated "
    "bit is enabled and the client and server device share a bond.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicValuehandle>", "Handle of the characteristic value to write" },
        { "<value>", "Hexadecimal string representation of the value to write" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command write {
    "write",
    "Write a characteristic value to a server.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicValuehandle>", "Handle of the characteristic value to write" },
        { "<value>", "Hexadecimal string representation of the value to write" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command writeLong {
    "writeLong",
    "Write a characteristic value to a server. This sub-procedure is used when "
    "the client knows the Characteristic Value Handle but the length of the "
    "Characteristic Value is longer than can be sent in a single Write Request "
    "Attribute Protocol message.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicValuehandle>", "Handle of the characteristic value to write" },
        { "<value>", "Hexadecimal string representation of the value to write" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command reliableWrite {
    "reliableWrite",
    "Write a characteristic value to a server. This sub-procedure is used when "
    "the client knows the Characteristic Value Handle, and assurance is required "
    "that the correct Characteristic Value is going to be written by transferring "
    "the Characteristic Value to be written in both directions before the write is "
    "performed. This sub-procedure can also be used when multiple values must be "
    "written, in order, in a single operation",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicValuehandle>", "Handle of the characteristic value to write" },
        { "<value>", "Hexadecimal string representation of the value to write" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command readCharacteristicDescriptor {
    "readCharacteristicDescriptor",
    "Read a characteristic descriptor from a server.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicDescriptorhandle>", "Handle of the characteristic descriptor to read" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command readLongCharacteristicDescriptor {
    "readLongCharacteristicDescriptor",
    "Read a characteristic descriptor from a server. This procedure is used "
    "when the length of the characteristic descriptor declaration is longer "
    "than what can be sent in a single read",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicDescriptorhandle>", "Handle of the characteristic descriptor to read" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command writeCharacteristicDescriptor {
    "writeCharacteristicDescriptor",
    "Write a characteristic descriptor to a server.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicDescriptorhandle>", "Handle of the characteristic descriptor to write" },
        { "<value>", "Hexadecimal string representation of the value to write" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

static constexpr const Command writeLongCharacteristicDescriptor {
    "writeLongCharacteristicDescriptor",
    "Write a characteristic descriptor to a server. This procedure when the "
    "client knows that the length of the characteristic descriptor value is "
    "longer than what can be sent in a single write.",
    (const CommandArgDescription[]) {
        { "<connectionHandle>", "The connection used by this procedure" },
        { "<characteristicDescriptorhandle>", "Handle of the characteristic descriptor to write" },
        { "<value>", "Hexadecimal string representation of the value to write" }
    },
    STATIC_LAMBDA(const CommandArgs&) {
        return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
    }
};

} // end of annonymous namespace

ConstArray<Command> GattClientCommandSuiteDescription::commands() {
    static constexpr const Command commandHandlers[] = {
        discoverAllServicesAndCharacteristics,
        discoverAllServices,
        discoverServices,
        findIncludedServices,
        discoverCharacteristicsOfService,
        discoverCharacteristicsByUUID,
        discoverAllCharacteristicsDescriptors,
        readCharacteristicValue,
        readUsingCharacteristicUUID,
        readLongCharacteristicValue,
        readMultipleCharacteristicValues,
        writeWithoutResponse,
        signedWriteWithoutResponse,
        write,
        writeLong,
        reliableWrite,
        readCharacteristicDescriptor,
        readLongCharacteristicDescriptor,
        writeCharacteristicDescriptor,
        writeLongCharacteristicDescriptor
    };

    return ConstArray<Command>(commandHandlers);
}
