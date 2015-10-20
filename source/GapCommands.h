#ifndef BLE_CLIAPP_GAP_COMMANDS_H_
#define BLE_CLIAPP_GAP_COMMANDS_H_

#include "CLICommandSuite.h"

#include "ble/BLE.h"
#include "ble/Gap.h"
#include "Stringify.h"

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


	static int setAddress(const Args&) {
		//ble_error_t setAddress(AddressType_t type, const Address_t address)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int getAddress(const Args&) {
		//ble_error_t getAddress(AddressType_t *typeP, Address_t address)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int getMinAdvertisingInterval(const Args&) {
		//uint16_t getMinAdvertisingInterval(void) const
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int getMinNonConnectableAdvertisingInterval(const Args&) {
		//uint16_t getMinNonConnectableAdvertisingInterval(void) const
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;	
	}

	static int getMaxAdvertisingInterval(const Args&) {
		//uint16_t getMaxAdvertisingInterval(void) const
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;	
	}

	static int stopAdvertising(const Args&) {
		// ble_error_t stopAdvertising(void)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;	
	}

	static int stopScan(const Args&) {
		// ble_error_t stopScan(void)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;	
	}

	static int connect(const Args&) {
		/*  ble_error_t connect(const Address_t           peerAddr,
                                Gap::AddressType_t        peerAddrType,
                                const ConnectionParams_t *connectionParams,
                                const GapScanningParams  *scanParams)
                                */
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;	
	}

	static int disconnect(const Args&) { 
		//ble_error_t disconnect(Handle_t connectionHandle, DisconnectionReason_t reason)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;	
	}

	static int getPreferredConnectionParams(const Args&) { 
		//ble_error_t getPreferredConnectionParams(ConnectionParams_t *params)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;		
	}

	static int setPreferredConnectionParams(const Args&) { 
		//ble_error_t setPreferredConnectionParams(const ConnectionParams_t *params)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;		
	}

	static int updateConnectionParams(const Args&) { 
		//ble_error_t updateConnectionParams(Handle_t handle, const ConnectionParams_t *params)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int setDeviceName(const Args&) { 
		//ble_error_t setDeviceName(const uint8_t *deviceName)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int getDeviceName(const Args&) { 
		//ble_error_t getDeviceName(uint8_t *deviceName, unsigned *lengthP)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int setAppearance(const Args&) { 
		//ble_error_t setAppearance(GapAdvertisingData::Appearance appearance)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int getAppearance(const Args&) { 
		//ble_error_t getAppearance(GapAdvertisingData::Appearance* appearance)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int setTxPower(const Args&) { 
		//ble_error_t setTxPower(int8_t txPower)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int getPermittedTxPowerValues(const Args&) { 
		// void getPermittedTxPowerValues(const int8_t **valueArrayPP, size_t *countP)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int getState(const Args&) { 
		// GapState_t getState(void) const
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int setAdvertisingType(const Args&) { 
		//void setAdvertisingType(GapAdvertisingParams::AdvertisingType_t advType)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int setAdvertisingInterval(const Args&) { 
		//	void setAdvertisingInterval(uint16_t interval)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int setAdvertisingTimeout(const Args&) { 
		//	void setAdvertisingTimeout(uint16_t timeout)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int startAdvertising(const Args&) { 
		//ble_error_t startAdvertising(void)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int clearAdvertisingPayload(const Args&) { 
		//void clearAdvertisingPayload(void)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int accumulateAdvertisingPayload(const Args&) { 
		//ble_error_t accumulateAdvertisingPayload(uint8_t flags)
		//ble_error_t accumulateAdvertisingPayload(GapAdvertisingData::Appearance app)
		//ble_error_t accumulateAdvertisingPayloadTxPower(int8_t power)
		//ble_error_t accumulateAdvertisingPayload(GapAdvertisingData::DataType type, const uint8_t *data, uint8_t len)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int updateAdvertisingPayload(const Args&) { 
		//ble_error_t updateAdvertisingPayload(GapAdvertisingData::DataType type, const uint8_t *data, uint8_t len)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int setAdvertisingPayload(const Args&) { 
		//ble_error_t setAdvertisingPayload(const GapAdvertisingData &payload)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int getAdvertisingPayload(const Args&) { 
		//const GapAdvertisingData &getAdvertisingPayload(void) const
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int accumulateScanResponse(const Args&) { 
		//ble_error_t accumulateScanResponse(GapAdvertisingData::DataType type, const uint8_t *data, uint8_t len) {
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int clearScanResponse(const Args&) { 
		//void clearScanResponse(void)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int setScanParams(const Args&) { 
		/*
		    ble_error_t setScanParams(uint16_t interval       = GapScanningParams::SCAN_INTERVAL_MAX,
		                              uint16_t window         = GapScanningParams::SCAN_WINDOW_MAX,
		                              uint16_t timeout        = 0,
		                              bool     activeScanning = false)
		*/
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int setScanInterval(const Args&) { 
		/*
			ble_error_t setScanInterval(uint16_t interval)
		*/
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int setScanWindow(const Args&) { 
		//ble_error_t setScanWindow(uint16_t window)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int setScanTimeout(const Args&) { 
		//ble_error_t setScanTimeout(uint16_t timeout)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int setActiveScanning(const Args&) { 
		//ble_error_t setActiveScanning(bool activeScanning)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int startScan(const Args&) { 
		//ble_error_t startScan(void (*callback)(const AdvertisementCallbackParams_t *params))
		//ble_error_t startScan(T *object, void (T::*callbackMember)(const AdvertisementCallbackParams_t *params))		
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int initRadioNotification(const Args&) { 
		//ble_error_t initRadioNotification(void)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int getAdvertisingParams(const Args&) { 
		//GapAdvertisingParams &getAdvertisingParams(void)
		return CMDLINE_RETCODE_COMMAND_NOT_IMPLEMENTED;
	}

	static int setAdvertisingParams(const Args&) { 
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