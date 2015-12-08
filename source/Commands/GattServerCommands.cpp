#include "GattServerCommands.h"
#include "util/StaticLambda.h"
#include "ble/BLE.h"
#include "ble/services/HeartRateService.h"
#include "Serialization/Serializer.h"

   
// isolation
namespace {

static HeartRateService *HRMService = nullptr;

static BLE& ble() {
    return BLE::Instance();
}

static constexpr const Command instantiateHRM { 
    "instantiateHRM",
    "instantiate an HRM service, this command will be removed in the future",
    (const CommandArgDescription[]) {
        { "<value>", "The hrm sensor value (uint16_t)" }
    }, 
    STATIC_LAMBDA(const CommandArgs& args) {
        if(HRMService) { 
            return CommandResult::faillure("The service has already been instantiated"_ss);
        }

        uint16_t sensorValue;
        if(!fromString(args[0], sensorValue)) { 
            return CommandResult::invalidParameters("The sensor value is ill formed"_ss);
        }

        HRMService = new HeartRateService(ble(), sensorValue, HeartRateService::LOCATION_FINGER); 

        return CommandResult::success();
    } 
};

static constexpr const Command updateHRMSensorValue { 
    "updateHRMSensorValue",
    "update the sensor value of the HRM service, this command will be removed in the future", 
    (const CommandArgDescription[]) {
        { "<value>", "The new sensor value (uint16_t)" }
    },
    STATIC_LAMBDA(const CommandArgs& args) {
        if(!HRMService) { 
            return CommandResult::faillure("The HRM service has not been instantiated"_ss);
        }

        uint16_t sensorValue;
        if(!fromString(args[0], sensorValue)) { 
            return CommandResult::invalidParameters("The sensor value is ill formed"_ss);
        }

        HRMService->updateHeartRate(sensorValue);

        return CommandResult::success();
    } 
};


} // end of annonymous namespace

ConstArray<Command> GattServerCommandSuiteDescription::commands() {
    static constexpr const Command commandHandlers[] = {
        instantiateHRM,
        updateHRMSensorValue
    };

    return ConstArray<Command>(commandHandlers);        
}
