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

static CommandResult getDeviceName(const CommandArgs&) { 
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

static CommandResult setAppearance(const CommandArgs& args) { 
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

static CommandResult getAppearance(const CommandArgs&) { 
	GapAdvertisingData::Appearance_t appearance = GapAdvertisingData::UNKNOWN;
	ble_error_t err = gap().getAppearance(&appearance);

	if(err) {
		return CommandResult::faillure(to_string(err));				
	}

	return CommandResult::success(toString(appearance));
}

static CommandResult setTxPower(const CommandArgs& args) { 
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

static CommandResult getPermittedTxPowerValues(const CommandArgs&) { 
	const int8_t* permittedTxPowerValues = NULL;
	size_t permittedTxPowerValuesCount = 0;

	gap().getPermittedTxPowerValues(&permittedTxPowerValues, &permittedTxPowerValuesCount);

	return CommandResult::success(txPermittedValuesToJSON(permittedTxPowerValues, permittedTxPowerValuesCount));
}

static CommandResult getState(const CommandArgs&) { 
	Gap::GapState_t state = gap().getState();
	return CommandResult::success(gapStateToJSON(state));	
}

static CommandResult setAdvertisingType(const CommandArgs& args) { 
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

static CommandResult setAdvertisingInterval(const CommandArgs& args) { 
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

static CommandResult setAdvertisingTimeout(const CommandArgs& args) { 
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

static CommandResult startAdvertising(const CommandArgs&) { 
	ble_error_t err = gap().startAdvertising();
	return err ? CommandResult::faillure(to_string(err)) : CommandResult::success();		
}

static CommandResult clearAdvertisingPayload(const CommandArgs&) { 
	gap().clearAdvertisingPayload();
	return CommandResult::success();
}

static CommandResult accumulateAdvertisingPayload(const CommandArgs& args) { 
	AdvertisingPayloadField_t payloadField;
	const char* parsingError = advertisingPayloadFieldFromCLI(args, payloadField);

	if(parsingError) {
		return CommandResult::invalidParameters(parsingError);
	}

	ble_error_t err = gap().accumulateAdvertisingPayload(payloadField.dataType, payloadField.data, payloadField.dataLenght);
	return err ? CommandResult::faillure(to_string(err)) : CommandResult::success();		
}

static CommandResult updateAdvertisingPayload(const CommandArgs& args) { 
	AdvertisingPayloadField_t payloadField;
	const char* parsingError = advertisingPayloadFieldFromCLI(args, payloadField);

	if(parsingError) {
		return CommandResult::invalidParameters(parsingError);
	}

	ble_error_t err = gap().updateAdvertisingPayload(payloadField.dataType, payloadField.data, payloadField.dataLenght);
	return err ? CommandResult::faillure(to_string(err)) : CommandResult::success();		
}

static CommandResult setAdvertisingPayload(const CommandArgs&) { 
	// TODO 
	//ble_error_t setAdvertisingPayload(const GapAdvertisingData &payload)
	return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
}

static CommandResult getAdvertisingPayload(const CommandArgs&) { 
	const GapAdvertisingData& advertisingData = gap().getAdvertisingPayload();
	return CommandResult::success(gapAdvertisingDataToJSON(advertisingData));
}

static CommandResult accumulateScanResponse(const CommandArgs& args) { 
	AdvertisingPayloadField_t payloadField;
	const char* parsingError = advertisingPayloadFieldFromCLI(args, payloadField);

	if(parsingError) {
		return CommandResult::invalidParameters(parsingError);
	}

	ble_error_t err = gap().accumulateScanResponse(payloadField.dataType, payloadField.data, payloadField.dataLenght);
	return err ? CommandResult::faillure(to_string(err)) : CommandResult::success();		
}

static CommandResult clearScanResponse(const CommandArgs&) { 
	gap().clearScanResponse();
	return CommandResult::success();		
}

static CommandResult setScanParams(const CommandArgs& args) {
	if(args.count() != 4) {
		return CommandResult::invalidParameters("arguments expected : <interval> <window> <timeout> <activeScanning>");		
	}

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

static CommandResult setScanInterval(const CommandArgs& args) { 
	if(args.count() != 1) {
		return CommandResult::invalidParameters("an interval is expected");
	}

	uint16_t interval = 0;
	if(!fromString(args[0], interval)) {
		return CommandResult::invalidParameters("the interval is ill formed");	
	}

	ble_error_t err = gap().setScanInterval(interval);
	return err ? CommandResult::faillure(to_string(err)) : CommandResult::success();		
}

static CommandResult setScanWindow(const CommandArgs& args) {
	if(args.count() != 1) {
		return CommandResult::invalidParameters("an scan window is expected");
	}

	uint16_t window = 0;
	if(!fromString(args[0], window)) {
		return CommandResult::invalidParameters("the window is ill formed");	
	}

	ble_error_t err = gap().setScanWindow(window);
	return err ? CommandResult::faillure(to_string(err)) : CommandResult::success();		
}

static CommandResult setScanTimeout(const CommandArgs& args) { 
	if(args.count() != 1) {
		return CommandResult::invalidParameters("a scan timeout is expected");
	}

	uint16_t timeout = 0;
	if(!fromString(args[0], timeout)) {
		return CommandResult::invalidParameters("the timeout is ill formed");	
	}

	ble_error_t err = gap().setScanTimeout(timeout);
	return err ? CommandResult::faillure(to_string(err)) : CommandResult::success();		
}

static CommandResult setActiveScanning(const CommandArgs& args) { 
	if(args.count() != 1) {
		return CommandResult::invalidParameters("the status of active scanning is expected");
	}

	bool activeScanning = 0;
	if(!fromString(args[0], activeScanning)) {
		return CommandResult::invalidParameters("the active scanning state is ill formed");	
	}

	ble_error_t err = gap().setActiveScanning(activeScanning);
	return err ? CommandResult::faillure(to_string(err)) : CommandResult::success();		
}

static CommandResult startScan(const CommandArgs&) { 
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


static CommandResult initRadioNotification(const CommandArgs&) { 
	// TODO (maybe)
	//ble_error_t initRadioNotification(void)
	return CommandResult(CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED);
}

static CommandResult getAdvertisingParams(const CommandArgs&) {
	GapAdvertisingParams& advertisingParams = gap().getAdvertisingParams();
	return CommandResult::success(advertisingParamsToJSON(advertisingParams));
}

static CommandResult setAdvertisingParams(const CommandArgs& args) { 
	if(args.count() != 3) {
		return CommandResult::invalidParameters("three parameters required <AdvertisingType_t> <interval> <timeout>");
	} 

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


/** 
 * references for callbacks : 
 *   - void onTimeout(TimeoutEventCallback_t callback)
 *   - void onConnection(ConnectionEventCallback_t callback)
 *   - template<typename T>
 *     void onConnection(T *tptr, void (T::*mptr)(const ConnectionCallbackParams_t*))
 *   - void removeAllOnConnectionCallbacks() 
 *   - void onDisconnection(DisconnectionEventCallback_t callback) 
 *   - template<typename T>
 *     void onDisconnection(T *tptr, void (T::*mptr)(const DisconnectionCallbackParams_t*)) 
 *   - void removeAllOnDisconnectionCallbacks() 
 *   - void onRadioNotification(void (*callback)(bool param))
 *   - template <typename T>
 *     void onRadioNotification(T *tptr, void (T::*mptr)(bool))
 */








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
		{ "getDeviceName", getDeviceName },
		{ "setAppearance", setAppearance },
		{ "getAppearance", getAppearance },
		{ "setTxPower", setTxPower },
		{ "getPermittedTxPowerValues", getPermittedTxPowerValues },
		{ "getState", getState },
		{ "setAdvertisingType", setAdvertisingType },
		{ "setAdvertisingInterval", setAdvertisingInterval },
		{ "setAdvertisingTimeout", setAdvertisingTimeout },
		{ "startAdvertising", startAdvertising },
		{ "clearAdvertisingPayload", clearAdvertisingPayload },
		{ "accumulateAdvertisingPayload", accumulateAdvertisingPayload },
		{ "updateAdvertisingPayload", updateAdvertisingPayload },
		{ "setAdvertisingPayload", setAdvertisingPayload },
		{ "getAdvertisingPayload", getAdvertisingPayload },
		{ "accumulateScanResponse", accumulateScanResponse },
		{ "clearScanResponse", clearScanResponse },
		{ "setScanParams", setScanParams },
		{ "setScanInterval", setScanInterval },
		{ "setScanWindow", setScanWindow },
		{ "setScanTimeout", setScanTimeout },
		{ "setActiveScanning", setActiveScanning },
		{ "startScan", startScan },
		{ "initRadioNotification", initRadioNotification },
		{ "getAdvertisingParams", getAdvertisingParams },
		{ "setAdvertisingParams", setAdvertisingParams }	
	};


	return ConstArray<Command>(commandHandlers);		
} 