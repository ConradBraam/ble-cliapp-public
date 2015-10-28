#ifndef BLE_CLIAPP_GAP_COMMANDS_H_
#define BLE_CLIAPP_GAP_COMMANDS_H_

#include "CLICommand/CLICommandSuite.h"

#include "ble/BLE.h"
#include "ble/Gap.h"
#include "Serialization/GapSerializer.h"
#include "Serialization/GapAdvertisingDataSerializer.h"
#include "Serialization/GapAdvertisingParamsSerializer.h"
#include "Serialization/BleCommonSerializer.h"


class GapCommandSuiteDescription {

public:
	static const char* name() { 
		return "gap";
	} 
 
	static const char* info() {
		return "All commands applicable to the Gap instance of this device";
	} 

	static const char* man() {
		return "gap <command> <command arguments>\r\n"\
		       "	* gap shutdown : shutdown the ble instance of this device\r\n";
	} 

	static ConstArray<Command_t> commands() {
		static const Command_t commandHandlers[] = {
			{ "setAddress", setAddress },
			{ "getAddress", getAddress },
			{ "getMinAdvertisingInterval", getMinAdvertisingInterval },
			{ "getMinNonConnectableAdvertisingInterval", getMinNonConnectableAdvertisingInterval },
			{ "getMaxAdvertisingInterval", getMaxAdvertisingInterval },
			{ "stopAdvertising", stopAdvertising },
			{ "stopScan", stopScan },
			{ "connect", connect },
			{ "disconnect", disconnect },
			{ "getPreferredConnectionParams", getPreferredConnectionParams },
			{ "setPreferredConnectionParams", setPreferredConnectionParams },
			{ "updateConnectionParams", updateConnectionParams },
			{ "setDeviceName", setDeviceName },
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


		return ConstArray<Command_t>(commandHandlers);		
	}


//private:

	/**
	 * @brief  set the address of this device 
	 * 
	 * @param args Two args should be provided to this command: 
	 *     - The address type which is a string representation of Gap::AddressType_t
	 *     - The address itself which is a string representation like "XX:XX:XX:XX:XX:XX"
	 * @return [description]
	 */
	static CommandResult setAddress(const CommandArgs& args) {
		// check args count
		if(args.count() != 2) {
			return CommandResult::invalidParameters("two arguments expected: <Gap::AddressType_t> <XX:XX:XX:XX:XX:XX>");
		}

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

		if(err) {
			return CommandResult::faillure(to_string(err));
		}


		return CommandResult::success();
	}

	static CommandResult getAddress(const CommandArgs& args) {
		namespace pj = picojson;

		// check args count
		if(args.count() != 0) {
			return CommandResult::invalidParameters("no arguments expected");
		}

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

	static CommandResult getMinAdvertisingInterval(const CommandArgs& args) {
		// check args count
		if(args.count() != 0) {
			return CommandResult::invalidParameters("no arguments expected");
		}

		uint16_t minAdvertisingInterval = gap().getMinAdvertisingInterval();

		return CommandResult::success(picojson::value((int64_t) minAdvertisingInterval));
	}

	static CommandResult getMinNonConnectableAdvertisingInterval(const CommandArgs& args) {
		// check args count
		if(args.count() != 0) {
			return CommandResult::invalidParameters("no arguments expected");
		}

		uint16_t minNonConnectableAdvertisingInterval = gap().getMinNonConnectableAdvertisingInterval();

		return CommandResult::success(picojson::value((int64_t) minNonConnectableAdvertisingInterval));
	}

	static CommandResult getMaxAdvertisingInterval(const CommandArgs& args) {
		// check args count
		if(args.count() != 0) {
			return CommandResult::invalidParameters("no arguments expected");
		}

		uint16_t maxAdvertisingInterval = gap().getMaxAdvertisingInterval();

		return CommandResult::success(picojson::value((int64_t) maxAdvertisingInterval));
	}

	static CommandResult stopAdvertising(const CommandArgs& args) {
		if(args.count() != 0) {
			return CommandResult::invalidParameters("no arguments expected");
		}

		ble_error_t err = gap().stopAdvertising();

		if(err) { 
			return CommandResult::faillure(to_string(err));
		}

		return CommandResult::success();
	}

	static CommandResult stopScan(const CommandArgs& args) {
		if(args.count() != 0) {
			return CommandResult::invalidParameters("no arguments expected");
		}

		ble_error_t err = gap().stopScan();

		if(err) { 
			return CommandResult::faillure(to_string(err));
		}

		return CommandResult::success();
	}

	static CommandResult connect(const CommandArgs&) {
		// TODO 
		/*  ble_error_t connect(const Address_t           peerAddr,
                                Gap::AddressType_t        peerAddrType,
                                const ConnectionParams_t *connectionParams,
                                const GapScanningParams  *scanParams)
                                */
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;	
	}

	static CommandResult disconnect(const CommandArgs&) { 
		// TODO 
		//ble_error_t disconnect(Handle_t connectionHandle, DisconnectionReason_t reason)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;	
	}

	static CommandResult getPreferredConnectionParams(const CommandArgs& args) { 
		if(args.count() != 0) {
			return CommandResult::invalidParameters("no arguments expected");
		}

		Gap::ConnectionParams_t connectionParameters;

		ble_error_t err = gap().getPreferredConnectionParams(&connectionParameters);

		if(err) { 
			return CommandResult::faillure(to_string(err));
		}

		return CommandResult::success(connectionParamsToJSON(connectionParameters));
	}

	static CommandResult setPreferredConnectionParams(const CommandArgs& args) { 
		if(args.count() != 1) {
			return CommandResult::invalidParameters("expected connection parameters in the form"\
				"<minConnectionInterval>,<maxConnectionInterval>,<slaveLatency>,<connectionSupervisionTimeout>");
		}

		Gap::ConnectionParams_t connectionParameters;
		if(!connectionParamsFromCLI(args[0], connectionParameters)) {
			return CommandResult::invalidParameters("malformed connection parameters, should be like"\
				"<minConnectionInterval>,<maxConnectionInterval>,<slaveLatency>,<connectionSupervisionTimeout>");
		}

		ble_error_t err =  gap().setPreferredConnectionParams(&connectionParameters);

		if(err) {
			return CommandResult::faillure(to_string(err));	
		}

		return CommandResult::success();		
	}

	static CommandResult updateConnectionParams(const CommandArgs&) {
		// TODO  
		//ble_error_t updateConnectionParams(Handle_t handle, const ConnectionParams_t *params)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static CommandResult setDeviceName(const CommandArgs& args) { 
		if(args.count() != 1) {
			return CommandResult::invalidParameters("please the name of the device without spaces");
		}

		ble_error_t err = gap().setDeviceName((const uint8_t*) args[0]);

		if(err) {
			return CommandResult::faillure(to_string(err));	
		}

		return CommandResult::success();		
	}

	static CommandResult getDeviceName(const CommandArgs& args) { 
		if(args.count() != 0) {
			return CommandResult::invalidParameters("no arguments expected");
		}

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
		
		if(err) {
			return CommandResult::faillure(to_string(err));	
		}

		return CommandResult::success();
	}

	static CommandResult getAppearance(const CommandArgs& args) { 
		if(args.count() != 0) {
			return CommandResult::invalidParameters("no arguments expected");
		}

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

		if(err) {
			return CommandResult::faillure(to_string(err));				
		}

		return CommandResult::success();
	}

	static CommandResult getPermittedTxPowerValues(const CommandArgs& args) { 
		if(args.count() != 0) {
			return CommandResult::invalidParameters("no arguments expected");
		}

		const int8_t* permittedTxPowerValues = NULL;
		size_t permittedTxPowerValuesCount = 0;

		gap().getPermittedTxPowerValues(&permittedTxPowerValues, &permittedTxPowerValuesCount);

		return CommandResult::success(txPermittedValuesToJSON(permittedTxPowerValues, permittedTxPowerValuesCount));
	}

	static CommandResult getState(const CommandArgs& args) { 
		if(args.count() != 0) {
			return CommandResult::invalidParameters("no arguments expected");
		}

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

	static CommandResult startAdvertising(const CommandArgs& args) { 
		if(args.count() != 0) {
			return CommandResult::invalidParameters("no arguments expected");
		}

		ble_error_t err = gap().startAdvertising();

		if(err) {
			return CommandResult::faillure(toString(err));
		}

		return CommandResult::success();
	}

	static CommandResult clearAdvertisingPayload(const CommandArgs& args) { 
		if(args.count() != 0) {
			return CommandResult::invalidParameters("no arguments expected");
		}

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

		if(err) { 
			return CommandResult::faillure(toString(err));
		}

		return CommandResult::success();
	}

	static CommandResult updateAdvertisingPayload(const CommandArgs& args) { 
		AdvertisingPayloadField_t payloadField;
		const char* parsingError = advertisingPayloadFieldFromCLI(args, payloadField);

		if(parsingError) {
			return CommandResult::invalidParameters(parsingError);
		}

		ble_error_t err = gap().updateAdvertisingPayload(payloadField.dataType, payloadField.data, payloadField.dataLenght);

		if(err) { 
			return CommandResult::faillure(toString(err));
		}

		return CommandResult::success();
	}

	static CommandResult setAdvertisingPayload(const CommandArgs&) { 
		// TODO 
		//ble_error_t setAdvertisingPayload(const GapAdvertisingData &payload)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static CommandResult getAdvertisingPayload(const CommandArgs& args) { 
		if(args.count() != 0) {
			return CommandResult::invalidParameters("no arguments expected");
		}

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

		if(err) { 
			return CommandResult::faillure(toString(err));
		}

		return CommandResult::success();
	}

	static CommandResult clearScanResponse(const CommandArgs& args) { 
		if(args.count() != 0) {
			return CommandResult::invalidParameters("no arguments expected");
		}

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

		if(err) {
			return CommandResult::faillure(toString(err));
		}

		return CommandResult::success();
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

		if(err) {
			return CommandResult::faillure(toString(err));
		}

		return CommandResult::success();
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

		if(err) {
			return CommandResult::faillure(toString(err));
		}

		return CommandResult::success();
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

		if(err) {
			return CommandResult::faillure(toString(err));
		}

		return CommandResult::success();
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

		if(err) {
			return CommandResult::faillure(toString(err));
		}

		return CommandResult::success();
	}

	static CommandResult startScan(const CommandArgs& args) { 
		if(args.count() != 0) {
			return CommandResult::invalidParameters("no arguments expected");
		} 

		ble_error_t err = gap().startScan(scanCallback);

		if(err) {
			return CommandResult::faillure(toString(err));
		}

		return CommandResult::success();
	}

	static void scanCallback(const Gap::AdvertisementCallbackParams_t* scanResult) {
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
	}

	static CommandResult initRadioNotification(const CommandArgs&) { 
		// TODO (maybe)
		//ble_error_t initRadioNotification(void)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static CommandResult getAdvertisingParams(const CommandArgs& args) {
		if(args.count() != 0) {
			return CommandResult::invalidParameters("no arguments expected");
		} 

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
};




#endif //BLE_CLIAPP_GAP_COMMANDS_H_