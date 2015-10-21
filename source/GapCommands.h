#ifndef BLE_CLIAPP_GAP_COMMANDS_H_
#define BLE_CLIAPP_GAP_COMMANDS_H_

#include "CLICommandSuite.h"

#include "ble/BLE.h"
#include "ble/Gap.h"
#include "Stringify.h"
#include "GapSerializer.h"
#include "GapAdvertisingDataSerializer.h"
#include "BleCommonSerializer.h"


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


private:

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

	static CommandResult setTxPower(const CommandArgs&) { 
		//ble_error_t setTxPower(int8_t txPower)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static CommandResult getPermittedTxPowerValues(const CommandArgs&) { 
		// void getPermittedTxPowerValues(const int8_t **valueArrayPP, size_t *countP)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static CommandResult getState(const CommandArgs&) { 
		// GapState_t getState(void) const
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static CommandResult setAdvertisingType(const CommandArgs&) { 
		//void setAdvertisingType(GapAdvertisingParams::AdvertisingType_t advType)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static CommandResult setAdvertisingInterval(const CommandArgs&) { 
		//	void setAdvertisingInterval(uint16_t interval)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static CommandResult setAdvertisingTimeout(const CommandArgs&) { 
		//	void setAdvertisingTimeout(uint16_t timeout)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static CommandResult startAdvertising(const CommandArgs&) { 
		//ble_error_t startAdvertising(void)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static CommandResult clearAdvertisingPayload(const CommandArgs&) { 
		//void clearAdvertisingPayload(void)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static CommandResult accumulateAdvertisingPayload(const CommandArgs&) { 
		//ble_error_t accumulateAdvertisingPayload(uint8_t flags)
		//ble_error_t accumulateAdvertisingPayload(GapAdvertisingData::Appearance app)
		//ble_error_t accumulateAdvertisingPayloadTxPower(int8_t power)
		//ble_error_t accumulateAdvertisingPayload(GapAdvertisingData::DataType type, const uint8_t *data, uint8_t len)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static CommandResult updateAdvertisingPayload(const CommandArgs&) { 
		//ble_error_t updateAdvertisingPayload(GapAdvertisingData::DataType type, const uint8_t *data, uint8_t len)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static CommandResult setAdvertisingPayload(const CommandArgs&) { 
		//ble_error_t setAdvertisingPayload(const GapAdvertisingData &payload)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static CommandResult getAdvertisingPayload(const CommandArgs&) { 
		//const GapAdvertisingData &getAdvertisingPayload(void) const
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static CommandResult accumulateScanResponse(const CommandArgs&) { 
		//ble_error_t accumulateScanResponse(GapAdvertisingData::DataType type, const uint8_t *data, uint8_t len) {
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static CommandResult clearScanResponse(const CommandArgs&) { 
		//void clearScanResponse(void)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static CommandResult setScanParams(const CommandArgs&) { 
		/*
		    ble_error_t setScanParams(uint16_t interval       = GapScanningParams::SCAN_INTERVAL_MAX,
		                              uint16_t window         = GapScanningParams::SCAN_WINDOW_MAX,
		                              uint16_t timeout        = 0,
		                              bool     activeScanning = false)
		*/
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static CommandResult setScanInterval(const CommandArgs&) { 
		/*
			ble_error_t setScanInterval(uint16_t interval)
		*/
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static CommandResult setScanWindow(const CommandArgs&) { 
		//ble_error_t setScanWindow(uint16_t window)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static CommandResult setScanTimeout(const CommandArgs&) { 
		//ble_error_t setScanTimeout(uint16_t timeout)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static CommandResult setActiveScanning(const CommandArgs&) { 
		//ble_error_t setActiveScanning(bool activeScanning)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static CommandResult startScan(const CommandArgs&) { 
		//ble_error_t startScan(void (*callback)(const AdvertisementCallbackParams_t *params))
		//ble_error_t startScan(T *object, void (T::*callbackMember)(const AdvertisementCallbackParams_t *params))		
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static CommandResult initRadioNotification(const CommandArgs&) { 
		//ble_error_t initRadioNotification(void)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static CommandResult getAdvertisingParams(const CommandArgs&) { 
		//GapAdvertisingParams &getAdvertisingParams(void)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static CommandResult setAdvertisingParams(const CommandArgs&) { 
		//void setAdvertisingParams(const GapAdvertisingParams &newParams)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
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