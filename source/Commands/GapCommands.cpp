#include "GapCommands.h"

#include "ble/BLE.h"
#include "ble/Gap.h"
#include "Serialization/GapSerializer.h"
#include "Serialization/GapAdvertisingDataSerializer.h"
#include "Serialization/GapAdvertisingParamsSerializer.h"
#include "Serialization/BleCommonSerializer.h"

// isolation ...
namespace {

/**
 * return the ble instance of this device
 */
static BLE& ble() {
	return BLE::Instance();
}

/**
 * return the Gap instance of this device
 */
static Gap& gap() {
	return ble().gap();
}

static const Command setAddress { 
	"setAddress",
	"set the address of this device",
	{{
		{ "<addressType>", "The address type to set. It is a string representation of Gap::AddressType_t" },
		{ "<address>"    , "The address itself which is a string representation like \"XX:XX:XX:XX:XX:XX\"" },		
	}},
	[] (const CommandArgs& args) {
		// extract first args 
		Gap::AddressType_t addressType;
		if(!fromString(args[0], addressType)) {
			return CommandResult::invalidParameters("first argument should match Gap::AddressType_t");
		}

		Gap::Address_t address;
		if(!macAddressFromString(args[1], address)) { 
			return CommandResult::invalidParameters("second argument should is a mac address which should match XX:XX:XX:XX:XX:XX format");
		}

		ble_error_t err = gap().setAddress(addressType, address);

		return err ? CommandResult::faillure(to_string(err)) : CommandResult::success();
	}
};

static const Command getAddress {
	"getAddress",
	"Get the address and the type of address of this device.\r\n"
	"The result will be a json object containing:\r\n"
	"	* 'address_type': <type of the address. It is a string representation of Gap::AddressType_t>\r\n"
	"	* 'address'     : <the address which is a string representation like 'XX:XX:XX:XX:XX:XX'>\r\n",
	[] (const CommandArgs&) {
		namespace pj = picojson;

		Gap::AddressType_t addressType;
		Gap::Address_t address;

		ble_error_t err = gap().getAddress(&addressType, address);

		if(err) { 
			return CommandResult::faillure(to_string(err));
		}

		// building the result object 
		pj::value res(pj::object_type, true);
		res.get<pj::object>()["address_type"] = pj::value(toString(addressType));
		res.get<pj::object>()["address"] = pj::value(macAddressToString(address).str);

		return CommandResult::success(res);
	}
};

static const Command getMinAdvertisingInterval {
	"getMinAdvertisingInterval",
	"Return the minimum advertising interval",
	[] (const CommandArgs&) {
		uint16_t minAdvertisingInterval = gap().getMinAdvertisingInterval();
		return CommandResult::success(picojson::value((int64_t) minAdvertisingInterval));
	}
};

static const Command getMinNonConnectableAdvertisingInterval {
	"getMinNonConnectableAdvertisingInterval",
	// TODO DOC
	[] (const CommandArgs&) {
		uint16_t minNonConnectableAdvertisingInterval = gap().getMinNonConnectableAdvertisingInterval();
		return CommandResult::success(picojson::value((int64_t) minNonConnectableAdvertisingInterval));
	}
};

static const Command getMaxAdvertisingInterval {
	"getMaxAdvertisingInterval",
	//TODO DOC
	[] (const CommandArgs&) {	
		uint16_t maxAdvertisingInterval = gap().getMaxAdvertisingInterval();
		return CommandResult::success(picojson::value((int64_t) maxAdvertisingInterval));
	}
};

static const Command stopAdvertising {
	"stopAdvertising",
	//TODO DOC 
	[] (const CommandArgs&) {
		ble_error_t err = gap().stopAdvertising();
		return err ? CommandResult::faillure(to_string(err)) : CommandResult::success();
	}
};

static const Command stopScan { 
	"stopScan",
	//TODO DOC
	[] (const CommandArgs&) {
		ble_error_t err = gap().stopScan();
		return err ? CommandResult::faillure(to_string(err)) : CommandResult::success();
	}
};

static const Command connect {
	"connect",
	// TODO DOC
	[] (const CommandArgs&) {
		// TODO 
		/*  ble_error_t connect(const Address_t           peerAddr,
	                            Gap::AddressType_t        peerAddrType,
	                            const ConnectionParams_t *connectionParams,
	                            const GapScanningParams  *scanParams)
	                            */
		return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);	
	}
};

static const Command disconnect {
	"disconnect",
	//TODO DOC
	[] (const CommandArgs&) { 
		// TODO 
		//ble_error_t disconnect(Handle_t connectionHandle, DisconnectionReason_t reason)
		return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);	
	}
}; 

static const Command getPreferredConnectionParams { 
	"getPreferredConnectionParams",
	// TODO DOC
	[] (const CommandArgs&) { 
		Gap::ConnectionParams_t connectionParameters;
		ble_error_t err = gap().getPreferredConnectionParams(&connectionParameters);

		if(err) { 
			return CommandResult::faillure(to_string(err));
		}
		return CommandResult::success(connectionParamsToJSON(connectionParameters));
	}
};

static const Command setPreferredConnectionParams {
	"setPreferredConnectionParams", 
	"set the prefered connection parameters",
	{{
		{ "<minConnectionInterval>,<maxConnectionInterval>,<slaveLatency>,<connectionSupervisionTimeout>", "all the parameters, coma separated" }
	}},
	//TODO: betrer args 
	[] (const CommandArgs& args) { 
		Gap::ConnectionParams_t connectionParameters;
		if(!connectionParamsFromCLI(args[0], connectionParameters)) {
			return CommandResult::invalidParameters("malformed connection parameters, should be like"\
				"<minConnectionInterval>,<maxConnectionInterval>,<slaveLatency>,<connectionSupervisionTimeout>");
		}

		ble_error_t err =  gap().setPreferredConnectionParams(&connectionParameters);
		return err ? CommandResult::faillure(to_string(err)) : CommandResult::success();
	}	
};

static const Command updateConnectionParams {
	"updateConnectionParams",
	// TODO DOC 
	[] (const CommandArgs&) {
		// TODO  
		//ble_error_t updateConnectionParams(Handle_t handle, const ConnectionParams_t *params)
		return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
	}
};

static const Command setDeviceName {
	"setDeviceName",
	"set the devce name",
	{{
		{ "<name>", "the name of the device, it should not have space" }
	}},
	[] (const CommandArgs& args) { 
		if(args.count() != 1) {
			return CommandResult::invalidParameters("please the name of the device without spaces");
		}

		ble_error_t err = gap().setDeviceName((const uint8_t*) args[0]);
		return err ? CommandResult::faillure(to_string(err)) : CommandResult::success();		
	}
}; 

static const Command getDeviceName { 
	"getDeviceName",
	"return the device name as a string",
	[] (const CommandArgs&) { 
		// first : collect the size of the name 
		unsigned deviceNameLength;
		ble_error_t err = gap().getDeviceName(NULL, &deviceNameLength);

		if(err) { 
			return CommandResult::faillure("impossible to get the lenght of the name");
		}

		// acquire the name
		++deviceNameLength;  // add null terminator to the length
		uint8_t* deviceName = static_cast<uint8_t*>(malloc(deviceNameLength));
		memset(deviceName, 0, deviceNameLength);
		err = gap().getDeviceName(deviceName, &deviceNameLength);

		if(err) {
			free(deviceName);
			return CommandResult::faillure(to_string(err));	
		}

		CommandResult res = CommandResult::success((const char*) deviceName);
		free(deviceName);
		return res;
	}
};

static const  Command setAppearance {
	"setAppearance",
	"set the appearance flag of the device",
	{{
		{ "<appearance>", "the appearance to set (see GapAdvertisingData::Appearance_t)" }
	}},
	[] (const CommandArgs& args) { 
		if(args.count() != 1) {
			return CommandResult::invalidParameters("this command need the appearance to set (see GapAdvertisingData::Appearance_t)");
		}

		GapAdvertisingData::Appearance_t appearance = GapAdvertisingData::UNKNOWN;
		if(!fromString(args[0], appearance)) {
			return CommandResult::invalidParameters("the appearance to set is illformed (see GapAdvertisingData::Appearance_t)");
		}

		ble_error_t err = gap().setAppearance(appearance);
		return err ? CommandResult::faillure(to_string(err)) : CommandResult::success();		
	}
};

static const Command getAppearance { 
	"getAppearance",
	"get the appearance of the device",
	[] (const CommandArgs&) { 
		GapAdvertisingData::Appearance_t appearance = GapAdvertisingData::UNKNOWN;
		ble_error_t err = gap().getAppearance(&appearance);

		if(err) {
			return CommandResult::faillure(to_string(err));				
		}

		return CommandResult::success(toString(appearance));
	}
};

static const Command setTxPower {
	"setTxPower",
	"set the transmission power of the device",
	{{
		{ "<TxPower>", "The transmission power, it is an integer between [-128:127]"}
	}},
	[] (const CommandArgs& args) { 
		if(args.count() != 1) {
			return CommandResult::invalidParameters("this command need the TX power to be set [-127:128]");
		}

		int8_t txPower = 0;
		if(!fromString(args[0], txPower)) { 
			return CommandResult::invalidParameters("the txPower is malformed (should be between [-127:128])");
		}

		ble_error_t err = gap().setTxPower(txPower);
		return err ? CommandResult::faillure(to_string(err)) : CommandResult::success();		
	}
};

static const Command getPermittedTxPowerValues { 
	"getPermittedTxPowerValues",
	"return an array of the authorized Tx power values",
	[] (const CommandArgs&) { 
		const int8_t* permittedTxPowerValues = NULL;
		size_t permittedTxPowerValuesCount = 0;

		gap().getPermittedTxPowerValues(&permittedTxPowerValues, &permittedTxPowerValuesCount);

		return CommandResult::success(txPermittedValuesToJSON(permittedTxPowerValues, permittedTxPowerValuesCount));
	}
};

static const Command getState{ 
	"getState",
	"return the state of the device as defined in Gap::GapState_t",
	[] (const CommandArgs&) { 
		Gap::GapState_t state = gap().getState();
		return CommandResult::success(gapStateToJSON(state));	
	}
};

static const Command setAdvertisingType { 
	"setAdvertisingType",
	"set the advertising type",
	{{
		{ "<advertisingType>", "The advertising type as defined in GapAdvertisingParams::AdvertisingType_t" }
	}},
	[] (const CommandArgs& args) { 
		if(args.count() != 1) {
			return CommandResult::invalidParameters("the advertising type should be provided (see GapAdvertisingParams::AdvertisingType_t)");
		}

		GapAdvertisingParams::AdvertisingType_t advType;

		if(!fromString(args[0], advType)) {
			return CommandResult::invalidParameters("the advertising type is incorrect (see GapAdvertisingParams::AdvertisingType_t)");	
		}

		gap().setAdvertisingType(advType);

		return CommandResult::success();
	}
};

static const Command setAdvertisingInterval {
	"setAdvertisingInterval",
	"set the advertising interval",
	{{
		{ "<interval>", "The interval in ms, if 0, the advertising is disabled" }
	}},
	[] (const CommandArgs& args) { 
		if(args.count() != 1) {
			return CommandResult::invalidParameters("the advertising interval should be provided");
		}

		uint16_t interval = 0;
		if(!fromString(args[0], interval)) {
			return CommandResult::invalidParameters("the advertising interval is ill formed");
		}

		gap().setAdvertisingInterval(interval);

		return CommandResult::success();
	}
};

static const Command setAdvertisingTimeout {
	"setAdvertisingTimeout",
	"set the advertising timeout, in seconds",
	{{
		{ "<timeout>", "An integer wich represent the advertising timeout in seconds [0x1 : 0x3FFF]. 0 disable the timeout" }
	}},
	[] (const CommandArgs& args) { 
		if(args.count() != 1) {
			return CommandResult::invalidParameters("the advertising timeout should be provided");
		}	

		uint16_t timeout = 0;
		if(!fromString(args[0], timeout)) {
			return CommandResult::invalidParameters("the advertising timeout is ill formed");
		}

		gap().setAdvertisingTimeout(timeout);

		return CommandResult::success();
	}
};

static const Command startAdvertising {
	"startAdvertising",
	"start the advertising",
	[] (const CommandArgs&) { 
		ble_error_t err = gap().startAdvertising();
		return err ? CommandResult::faillure(to_string(err)) : CommandResult::success();		
	}
};

static const Command clearAdvertisingPayload {
	"clearAdvertisingPayload",
	"clear the advertising payload",
	[] (const CommandArgs&) { 
		gap().clearAdvertisingPayload();
		return CommandResult::success();
	}
};


static const Command accumulateAdvertisingPayload {
	"accumulateAdvertisingPayload",
	"add a new field into the advertising payload",
	{{
		{ "<fieldType>", "the field type of the data following (see GapAdvertisingData::DataType_t)" },
		{ "<data>", "the value of the field, please see GapAdvertisingData" }
	}},
	/* maximum args counts is undefined */ 0xFF,
	[] (const CommandArgs& args) { 
		AdvertisingPayloadField_t payloadField;
		const char* parsingError = advertisingPayloadFieldFromCLI(args, payloadField);

		if(parsingError) {
			return CommandResult::invalidParameters(parsingError);
		}

		ble_error_t err = gap().accumulateAdvertisingPayload(payloadField.dataType, payloadField.data, payloadField.dataLenght);
		return err ? CommandResult::faillure(to_string(err)) : CommandResult::success();		
	}
};

static const Command updateAdvertisingPayload {
	"updateAdvertisingPayload",
	"update a field int the advertising payload.\r\n"
	"Take care, at the moment, this will only succeed if the new value has the same size as the old one",
	{{
		{ "<fieldType>", "the field type to update (see GapAdvertisingData::DataType_t)" },
		{ "<data>", "the value of the field, it should have the same size as the previous value. please see GapAdvertisingData" }
	}},
	/* maximum args counts is undefined */ 0xFF,
	[] (const CommandArgs& args) { 
		AdvertisingPayloadField_t payloadField;
		const char* parsingError = advertisingPayloadFieldFromCLI(args, payloadField);

		if(parsingError) {
			return CommandResult::invalidParameters(parsingError);
		}

		ble_error_t err = gap().updateAdvertisingPayload(payloadField.dataType, payloadField.data, payloadField.dataLenght);
		return err ? CommandResult::faillure(to_string(err)) : CommandResult::success();		
	}
};

static const Command setAdvertisingPayload {
	"setAdvertisingPayload",
	"set the advertising payload",
	[] (const CommandArgs&) { 
		// TODO 
		//ble_error_t setAdvertisingPayload(const GapAdvertisingData &payload)
		return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
	}
};

static const Command getAdvertisingPayload {
	"getAdvertisingPayload",
	"get the current advertising payload",
	[] (const CommandArgs&) { 
		const GapAdvertisingData& advertisingData = gap().getAdvertisingPayload();
		return CommandResult::success(gapAdvertisingDataToJSON(advertisingData));
	}
};

static const Command accumulateScanResponse {
	"accumulateScanResponse",
	"add a field into the scan response payload",
	{{
		{ "<fieldType>", "the field type to update (see GapAdvertisingData::DataType_t)" },
		{ "<data>", "the value of the field, it should have the same size as the previous value. please see GapAdvertisingData" }
	}},
	/* maximum args counts is undefined */ 0xFF,
	[] (const CommandArgs& args) { 
		AdvertisingPayloadField_t payloadField;
		const char* parsingError = advertisingPayloadFieldFromCLI(args, payloadField);

		if(parsingError) {
			return CommandResult::invalidParameters(parsingError);
		}

		ble_error_t err = gap().accumulateScanResponse(payloadField.dataType, payloadField.data, payloadField.dataLenght);
		return err ? CommandResult::faillure(to_string(err)) : CommandResult::success();		
	}
};

static const Command clearScanResponse {
	"clearScanResponse",
	"clear the scan response",
	[] (const CommandArgs&) { 
		gap().clearScanResponse();
		return CommandResult::success();
	}
};

static const Command setScanParams {
	"setScanParams",
	"Set the scan parameters",
	{{
		{ "<interval>", "The scan interval, it should be a number between 3 and 10420ms" },
		{ "<window>", "The scan window, it should be a number between 3 and 10420ms" },
		{ "<timeout>", "The scan timeout, it should be a number between 0 and 65534 " },
		{ "<activeScanning>", "A boolean value { true, false } indeicating if the device send scan request or not" }
	}},
	[] (const CommandArgs& args) {
		uint16_t interval = 0xFFFF;
		if(!fromString(args[0], interval)) {
			return CommandResult::invalidParameters("invalid interval, it should be a number between 3 and 10240ms");			 
		}

		uint16_t window = 0xFFFF;
		if(!fromString(args[1], window)) {	
			return CommandResult::invalidParameters("invalid window, it should be a number between 3 and 10240ms");
		}

		uint16_t timeout = 0;
		if(!fromString(args[2], timeout)) {	
			return CommandResult::invalidParameters("invalid timeout, it should be a number between 0 and 65534");
		}

		uint16_t activeScanning = 0;
		if(!fromString(args[3], activeScanning)) {	
			return CommandResult::invalidParameters("invalid activeScaning, it should be a number boolean value");
		}

		ble_error_t err = gap().setScanParams(interval, window, timeout, activeScanning);
		return err ? CommandResult::faillure(to_string(err)) : CommandResult::success();		
	}
};

static const Command setScanInterval {
	"setScanInterval",
	"set the scan interval",
	{{
		{ "<interval>", "The interval between each scan, it should be a number between 3 and 10240ms" }
	}},
	[] (const CommandArgs& args) { 
		uint16_t interval = 0;
		if(!fromString(args[0], interval)) {
			return CommandResult::invalidParameters("the interval is ill formed");	
		}

		ble_error_t err = gap().setScanInterval(interval);
		return err ? CommandResult::faillure(to_string(err)) : CommandResult::success();		
	}
};

static const Command setScanWindow {
	"setScanWindow",
	"set the scan windows",
	{{
		{ "<window>", "The interval between each scan, it should be a number between 3 and 10240ms" }
	}},
	[] (const CommandArgs& args) {
		uint16_t window = 0;
		if(!fromString(args[0], window)) {
			return CommandResult::invalidParameters("the window is ill formed");	
		}

		ble_error_t err = gap().setScanWindow(window);
		return err ? CommandResult::faillure(to_string(err)) : CommandResult::success();		
	}
};

static const Command setScanTimeout {
	"setScanTimeout",
	"The scan timeout",
	{{
		{ "<timeout>", "The scan timeout, it should be a number between 0 and 65534 " }
	}},
	[] (const CommandArgs& args) { 
		uint16_t timeout = 0;
		if(!fromString(args[0], timeout)) {
			return CommandResult::invalidParameters("the timeout is ill formed");	
		}

		ble_error_t err = gap().setScanTimeout(timeout);
		return err ? CommandResult::faillure(to_string(err)) : CommandResult::success();		
	}
};

static const Command setActiveScanning {
	"setActiveScanning",
	"Enable or disable active scanning",
	{{
		{ "<activeScanning>", "A boolean value { true, false } indeicating if the device send scan request or not" }
	}},
	[] (const CommandArgs& args) { 
		bool activeScanning = 0;
		if(!fromString(args[0], activeScanning)) {
			return CommandResult::invalidParameters("the active scanning state is ill formed");	
		}

		ble_error_t err = gap().setActiveScanning(activeScanning);
		return err ? CommandResult::faillure(to_string(err)) : CommandResult::success();		
	}
};

static const Command startScan {
	"startScan",
	"start the scan process",
	[] (const CommandArgs&) { 
		ble_error_t err = gap().startScan([] (const Gap::AdvertisementCallbackParams_t* scanResult) {
			//gap().stopScan();
			{
				picojson::value result(picojson::object_type, true);
				result.get<picojson::object>()["peerAddr"] = picojson::value(macAddressToString(scanResult->peerAddr).str);
				result.get<picojson::object>()["rssi"] = picojson::value((int64_t) scanResult->rssi);
				result.get<picojson::object>()["isScanResponse"] = picojson::value(scanResult->isScanResponse);
				result.get<picojson::object>()["type"] = picojson::value(toString(scanResult->type));
				result.get<picojson::object>()["data"] = gapAdvertisingDataToJSON(scanResult->advertisingData, scanResult->advertisingDataLen);

				//CLICommandSuite<GapCommandSuiteDescription>::commandReady("startScan", CommandArgs(0, 0), CommandResult::success(result));
			}
		});

		// TODO : use CONTINUE STATEMENT !!!!
		return err ? CommandResult::faillure(to_string(err)) : CommandResult::success();		
	}
};

static const Command initRadioNotification {
	"initRadioNotification",
	"initialize radio notifications",
	[] (const CommandArgs&) { 
		// TODO (maybe)
		//ble_error_t initRadioNotification(void)
		return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
	}
};

static const Command getAdvertisingParams {
	"getAdvertisingParams",
	"return the current advertising params",
	[] (const CommandArgs&) {
		GapAdvertisingParams& advertisingParams = gap().getAdvertisingParams();
		return CommandResult::success(advertisingParamsToJSON(advertisingParams));
	}
};

static const Command setAdvertisingParams {
	"setAdvertisingParams",
	"set the advertising parameters",
	{{
		{ "<advertisingType>", "The Advertising type, please refer to GapAdvertisingParams::AdvertisingType_t" },
		{ "<interval>", "The advertising interval, it should be a number between 0 and 65534" },
		{ "<timeout>", "The timeout, it should be a number between 0 and 65534" },
	}},
	[] (const CommandArgs& args) { 
		GapAdvertisingParams::AdvertisingType_t advertisingType;
		if(!fromString(args[0], advertisingType)) {
			return CommandResult::invalidParameters("Advertising type is malformed, please refer to GapAdvertisingParams::AdvertisingType_t");
		}

		uint16_t interval;
		if(!fromString(args[1], interval)) {
			return CommandResult::invalidParameters("Advertising interval is malformed, should be a number between 0 and 65534");
		}

		uint16_t timeout;		
		if(!fromString(args[2], timeout)) {
			return CommandResult::invalidParameters("Advertising timeout is malformed, should be a number between 0 and 65534");
		}

		gap().setAdvertisingParams(GapAdvertisingParams(advertisingType, interval, timeout));
		return CommandResult::success();
	}
};

} // end of annonymous namespace

ConstArray<Command> GapCommandSuiteDescription::commands() {
	static const Command commandHandlers[] = {
		setAddress,
		getAddress,
		getMinAdvertisingInterval,
		getMinNonConnectableAdvertisingInterval,
		getMaxAdvertisingInterval,
		stopAdvertising,
		stopScan,
		connect,
		disconnect,
		getPreferredConnectionParams,
		setPreferredConnectionParams,
		updateConnectionParams,
		setDeviceName,
		getDeviceName,
		setAppearance,
		getAppearance,
		setTxPower,
		getPermittedTxPowerValues,
		getState,
		setAdvertisingType,
		setAdvertisingInterval,
		setAdvertisingTimeout,
		startAdvertising,
		clearAdvertisingPayload,
		accumulateAdvertisingPayload,
		updateAdvertisingPayload,
		setAdvertisingPayload,
		getAdvertisingPayload,
		accumulateScanResponse,
		clearScanResponse,
		setScanParams,
		setScanInterval,
		setScanWindow,
		setScanTimeout,
		setActiveScanning,
		startScan,
		initRadioNotification,
		getAdvertisingParams,
		setAdvertisingParams	
	};

	return ConstArray<Command>(commandHandlers);		
} 