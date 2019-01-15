

#include "ble/BLE.h"
#include "ble/gap/Gap.h"
#include "Serialization/GapSerializer.h"
#include "Serialization/GapAdvertisingDataSerializer.h"
#include "Serialization/GapAdvertisingParamsSerializer.h"
#include "Serialization/BLECommonSerializer.h"
#include "CLICommand/CommandSuite.h"
#include "CLICommand/util/AsyncProcedure.h"
#include "Common.h"
#include "CLICommand/CommandHelper.h"

#ifdef YOTTA_CFG
#include "mbed-drivers/Timer.h"
#else
#include "Timer.h"
#endif

#include "GapV2Commands.h"

#include "parameters/AdvertisingParameters.h"
#include "parameters/ScanParameters.h"
#include "parameters/ConnectionParameters.h"
#include "Serialization/Hex.h"

using mbed::util::SharedPointer;

typedef BLEProtocol::AddressType_t LegacyAddressType_t;

// isolation ...
namespace {

const ble::AdvertisingParameters &getAdvertisingParameters() {
    return AdvertisingParametersCommandSuiteDescription::get();
}

const ble::ScanParameters &getScanParameters() {
    return ScanParametersCommandSuiteDescription::get();
}

const ble::ConnectionParameters &getConnectionParameters() {
    return ConnectionParametersCommandSuiteDescription::get();
}

void enable_event_handling() {
    using namespace serialization;

    struct EventHandler : public ble::Gap::EventHandler {
        virtual void onScanRequestReceived(const ble::ScanRequestEvent &event)
        {
            JSONEventStream() << startObject <<
                key("type") << "event" <<
                key("name") << "scan_request_received" <<
                key("value") << startObject <<
                    key("peer_address") << event.getPeerAddress() <<
                    key("peer_address_type") << event.getPeerAddressType() <<
                    key("advertising_handle") << event.getAdvHandle() <<
                endObject <<
            endObject;
        }

        virtual void onAdvertisingEnd(const ble::AdvertisingEndEvent &event)
        {
            JSONEventStream output;
            output << startObject <<
                key("type") << "event" <<
                key("name") << "advertising_end" <<
                key("value") << startObject <<
                    key("advertising_handle") << event.getAdvHandle() <<
                    key("completed_events") << event.getCompleted_events() <<
                    key("is_connected") << event.isConnected();
            if (event.isConnected()) {
                output <<
                    key("connection_handle") << event.getConnection();
            }
            output <<
                endObject <<
            endObject;
        }

        virtual void onAdvertisingReport(const ble::AdvertisingReportEvent &event)
        {
            JSONEventStream os;

            os << startObject <<
                key("type") << "event" <<
                key("name") << "advertising_report" <<
                key("value") << startObject <<
                    key("connectable") << event.getType().connectable() <<
                    key("scannable") << event.getType().scannable_advertising() <<
                    key("scan_response") << event.getType().scan_response() <<
                    key("directed") << event.getType().directed_advertising() <<
                    key("legacy") << event.getType().legacy_advertising() <<
                    key("rssi") << event.getRssi() <<
                    key("peer_address_type") << event.getPeerAddressType();

            if (event.getPeerAddressType() != ble::peer_address_type_t::ANONYMOUS) {
                os << key("peer_address") << event.getPeerAddress();
            }

            if (event.getType().directed_advertising()) {
                os << key("direct_address_type") << event.getDirectAddressType() <<
                      key("direct_address") << event.getDirectAddress();
            }

            if (event.getType().legacy_advertising() == false) {
                os << key("sid") << event.getSID() <<
                      key("tx_power") << event.getTxPower() <<
                      key("primary_phy") << event.getPrimaryPhy() <<
                      key("secondary_phy") << event.getSecondaryPhy() <<
                      key("data_status");

                switch (event.getType().data_status().value()) {
                    case ble::advertising_data_status_t::COMPLETE:
                        os << "COMPLETE";
                        break;
                    case ble::advertising_data_status_t::INCOMPLETE_MORE_DATA:
                        os << "INCOMPLETE_MORE_DATA";
                        break;
                    case ble::advertising_data_status_t::INCOMPLETE_DATA_TRUNCATED:
                        os << "INCOMPLETE_DATA_TRUNCATED";
                        break;
                    default:
                        os << "unknown";
                        break;
                }

                if (event.isPeriodicIntervalPresent()) {
                    os << key("periodic_interval") << event.getPeriodicInterval();
                }
            }

            os <<   key("payload") << event.getPayload() <<
                endObject <<
            endObject;
        }

        virtual void onScanTimeout(const ble::ScanTimeoutEvent &event)
        {
            JSONEventStream() << startObject <<
                key("type") << "event" <<
                key("name") << "scan_timeout" <<
            endObject;
        }

        virtual void onPeriodicAdvertisingSyncEstablished(
            const ble::PeriodicAdvertisingSyncEstablishedEvent &event
        )
        {
            JSONEventStream output;

            output << startObject <<
                key("type") << "event" <<
                key("name") << "periodic_advertising_sync_established" <<
                key("value") << startObject <<
                    key("status") << event.getStatus();

            if (event.getStatus() == BLE_ERROR_NONE) {
                output <<
                    key("peer_address_type") << event.getPeerAddressType() <<
                    key("peer_address") << event.getPeerAddress() <<
                    key("sync_handle") << event.getSyncHandle() <<
                    key("advertising_interval") << event.getAdvertisingInterval() <<
                    key("peer_phy") << event.getPeerPhy() <<
                    key("sid") << event.getSid() <<
                    key("peer_clock_accuracy") << ble::clock_accuracy_t(event.getPeerClockAccuracy()).get_ppm();
            }

            output <<
                endObject <<
            endObject;
        }

        virtual void onPeriodicAdvertisingReport(
            const ble::PeriodicAdvertisingReportEvent &event
        )
        {
            JSONEventStream() << startObject <<
                key("type") << "event" <<
                key("name") << "periodic_advertising_report" <<
                key("value") << startObject <<
                    key("sync_handle") << event.getSyncHandle() <<
                    key("rssi") << event.getRssi() <<
                    key("tx_power") << event.getTxPower() <<
                    key("data_status") << event.getDataStatus() <<
                    key("data") << event.getPayload() <<
                endObject <<
            endObject;
        }

        virtual void onPeriodicAdvertisingSyncLoss(
            const ble::PeriodicAdvertisingSyncLoss &event
        )
        {
            JSONEventStream() << startObject <<
                key("type") << "event" <<
                key("name") << "periodic_advertising_sync_loss" <<
                key("value") << startObject <<
                    key("sync_handle") << event.getSyncHandle() <<
                endObject <<
            endObject;
        }

        virtual void onConnectionComplete(const ble::ConnectionCompleteEvent &event)
        {
            JSONEventStream os;

            os << startObject <<
                key("type") << "event" <<
                key("name") << "connection_complete" <<
                key("value") << startObject <<
                    key("status") << event.getStatus();

            if (event.getStatus() != BLE_ERROR_NONE) {
                os << endObject << endObject;
                return;
            }

            os <<   key("peer_address_type") << event.getPeerAddressType() <<
                    key("peer_address") << event.getPeerAddress() <<
                    key("interval") << event.getConnectionInterval() <<
                    key("latency") << event.getConnectionLatency().value() <<
                    key("supervision_timeout") << event.getSupervisionTimeout() <<
                    key("connection_handle") << event.getConnectionHandle() <<
                    key("own_role") << (event.getOwnRole() == ble::connection_role_t::CENTRAL ? "CENTRAL" : "PERIPHERAL") <<
                    key("master_clock_accuracy") << event.getMasterClockAccuracy();

            if (event.getPeerResolvablePrivateAddress() != ble::address_t()) {
                os << key("peer_resolvable_private_address") << event.getPeerResolvablePrivateAddress();
            }

            if (event.getLocalResolvablePrivateAddress() != ble::address_t()) {
                os << key("local_resolvable_private_address") << event.getLocalResolvablePrivateAddress();
            }

            os << endObject <<
            endObject;
        }

        virtual void onUpdateConnectionParametersRequest(
            const ble::UpdateConnectionParametersRequestEvent &event
        )
        {
            JSONEventStream() << startObject <<
                key("type") << "event" <<
                key("name") << "update_connection_parameters_request" <<
                key("value") << startObject <<
                    key("connection_handle") << event.getConnectionHandle() <<
                    key("min_connection_interval") << event.getMinConnectionInterval() <<
                    key("max_connection_interval") << event.getMaxConnectionInterval() <<
                    key("slave_latency") << event.getSlaveLatency().value() <<
                    key("supervision_timeout") << event.getSupervisionTimeout() <<
                endObject <<
            endObject;
        }

        virtual void onConnectionParametersUpdateComplete(
            const ble::ConnectionParametersUpdateCompleteEvent &event
        )
        {
            JSONEventStream os;

            os << startObject <<
                key("type") << "event" <<
                key("name") << "on_connection_parameters_update_complete" <<
                key("value") << startObject <<
                    key("connection_handle") << event.getConnectionHandle() <<
                    key("status") << event.getStatus();

            if (event.getStatus() == BLE_ERROR_NONE) {
                os <<
                    key("connection_interval") << event.getConnectionInterval() <<
                    key("slave_latency") << event.getSlaveLatency().value() <<
                    key("supervision_timeout") << event.getSupervisionTimeout();
            }

            os <<
                endObject <<
            endObject;
        }

        virtual void onDisconnectionComplete(const ble::DisconnectionCompleteEvent &event)
        {
            JSONEventStream() << startObject <<
                key("type") << "event" <<
                key("name") << "disconnection_complete" <<
                key("value") << startObject <<
                    key("connection_handle") << event.getConnectionHandle() <<
                    key("reason") << event.getReason() <<
                endObject <<
            endObject;

        }

        virtual void onReadPhy(
            ble_error_t status,
            ble::connection_handle_t connectionHandle,
            ble::phy_t txPhy,
            ble::phy_t rxPhy
        )
        {
            JSONEventStream os;
            os << startObject <<
                key("type") << "event" <<
                key("name") << "read_phy" <<
                key("value") << startObject <<
                    key("connection_handle") << connectionHandle <<
                    key("status") << status;
            if (status == BLE_ERROR_NONE) {
                os <<
                    key("tx_phy") << txPhy <<
                    key("rx_phy") << rxPhy;
            }
            os <<
                endObject <<
            endObject;
        }

        virtual void onPhyUpdateComplete(
            ble_error_t status,
            ble::connection_handle_t connectionHandle,
            ble::phy_t txPhy,
            ble::phy_t rxPhy
        )
        {
            JSONEventStream os;
            os << startObject <<
                key("type") << "event" <<
                key("name") << "phy_update_complete" <<
                key("value") << startObject <<
                    key("connection_handle") << connectionHandle <<
                    key("status") << status;
            if (status == BLE_ERROR_NONE) {
                os <<
                    key("tx_phy") << txPhy <<
                    key("rx_phy") << rxPhy;
            }
            os <<
                endObject <<
            endObject;
        }
    };

    static EventHandler handler;

    gap().setEventHandler(&handler);
}

DECLARE_CMD(GetMaxAdvertisingSetNumber) {
    CMD_NAME("getMaxAdvertisingSetNumber")
    CMD_HANDLER(CommandResponsePtr& response) {
        response->success(gap().getMaxAdvertisingSetNumber());
    }
};

DECLARE_CMD(GetMaxAdvertisingDataLength) {
    CMD_NAME("getMaxAdvertisingDataLength")
    CMD_HANDLER(CommandResponsePtr& response) {
        response->success(gap().getMaxAdvertisingDataLength());
    }
};

DECLARE_CMD(CreateAdvertisingSet) {
    CMD_NAME("createAdvertisingSet")
    CMD_HANDLER(CommandResponsePtr& response) {
        ble::advertising_handle_t handle;
        ble_error_t err = gap().createAdvertisingSet(&handle, getAdvertisingParameters());
        reportErrorOrSuccess(response, err, handle);
    }
};

DECLARE_CMD(DestroyAdvertisingSet) {
    CMD_NAME("destroyAdvertisingSet")
    CMD_HANDLER(ble::advertising_handle_t handle, CommandResponsePtr& response) {
        ble_error_t err = gap().destroyAdvertisingSet(handle);
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(SetAdvertisingParameters) {
    CMD_NAME("setAdvertisingParameters")
    CMD_HANDLER(ble::advertising_handle_t handle, CommandResponsePtr& response) {
        ble_error_t err = gap().setAdvertisingParameters(handle, getAdvertisingParameters());
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(SetAdvertisingPayload) {
    CMD_NAME("setAdvertisingPayload")
    CMD_HANDLER(ble::advertising_handle_t handle, RawData_t& data, CommandResponsePtr& response) {
        ble_error_t err = gap().setAdvertisingPayload(handle, mbed::make_Span(data.cbegin(), data.size()));
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(SetAdvertisingScanResponse) {
    CMD_NAME("setAdvertisingScanResponse")
    CMD_HANDLER(ble::advertising_handle_t handle, RawData_t& data, CommandResponsePtr& response) {
        ble_error_t err = gap().setAdvertisingPayload(handle, mbed::make_Span(data.cbegin(), data.size()));
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(StartAdvertising) {
    CMD_NAME("startAdvertising")
    CMD_HANDLER(ble::advertising_handle_t handle, ble::adv_duration_t duration, uint8_t maxEvents, CommandResponsePtr& response) {
        ble_error_t err = gap().startAdvertising(handle, duration, maxEvents);
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(StopAdvertising) {
    CMD_NAME("stopAdvertising")
    CMD_HANDLER(ble::advertising_handle_t handle, CommandResponsePtr& response) {
        ble_error_t err = gap().stopAdvertising(handle);
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(IsAdvertisingActive) {
    CMD_NAME("isAdvertisingActive")
    CMD_HANDLER(ble::advertising_handle_t handle, CommandResponsePtr& response) {
        bool active = gap().isAdvertisingActive(handle);
        response->success(active);
    }
};

DECLARE_CMD(SetPeriodicAdvertisingParameters) {
    CMD_NAME("setPeriodicAdvertisingParameters")
    CMD_HANDLER(
        ble::advertising_handle_t handle,
        ble::periodic_interval_t min,
        ble::periodic_interval_t max,
        bool advertiseTxPower,
        CommandResponsePtr& response
    )
    {
        ble_error_t err = gap().setPeriodicAdvertisingParameters(
            handle,
            min,
            max,
            advertiseTxPower
        );

        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(SetPeriodicAdvertisingPayload) {
    CMD_NAME("setPeriodicAdvertisingPayload")
    CMD_HANDLER(ble::advertising_handle_t handle, RawData_t& data, CommandResponsePtr& response) {
        ble_error_t err = gap().setPeriodicAdvertisingPayload(handle, mbed::make_Span(data.cbegin(), data.size()));
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(StartPeriodicAdvertising) {
    CMD_NAME("startPeriodicAdvertising")
    CMD_HANDLER(ble::advertising_handle_t handle, CommandResponsePtr& response) {
        ble_error_t err = gap().startPeriodicAdvertising(handle);
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(StopPeriodicAdvertising) {
    CMD_NAME("stopPeriodicAdvertising")
    CMD_HANDLER(ble::advertising_handle_t handle, CommandResponsePtr& response) {
        ble_error_t err = gap().stopPeriodicAdvertising(handle);
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(IsPeriodicAdvertisingActive) {
    CMD_NAME("isPeriodicAdvertisingActive")
    CMD_HANDLER(ble::advertising_handle_t handle, CommandResponsePtr& response) {
        bool active = gap().isPeriodicAdvertisingActive(handle);
        response->success(active);
    }
};

DECLARE_CMD(SetScanParameters) {
    CMD_NAME("setScanParameters")
    CMD_HANDLER(CommandResponsePtr& response) {
        ble_error_t err = gap().setScanParameters(getScanParameters());
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(StartScan) {
    CMD_NAME("startScan")
    CMD_HANDLER(
        ble::scan_duration_t duration,
        ble::duplicates_filter_t::type filter,
        ble::scan_period_t period,
        CommandResponsePtr& response
    )
    {
        ble_error_t err = gap().startScan(duration, filter, period);
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(StopScan) {
    CMD_NAME("stopScan")
    CMD_HANDLER(CommandResponsePtr& response) {
        ble_error_t err = gap().stopScan();
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(CreateSync) {
    CMD_NAME("createSync")
    CMD_HANDLER(
        ble::peer_address_type_t::type peerAddressType,
        ble::address_t &peerAddress,
        uint8_t sid,
        uint16_t maxPacketSkip,
        ble::sync_timeout_t timeout,
        CommandResponsePtr& response
    )
    {
        ble_error_t err = gap().createSync(
            peerAddressType,
            peerAddress,
            sid,
            maxPacketSkip,
            timeout
        );
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(CreateSyncFromList) {
    CMD_NAME("createSyncFromList")
    CMD_HANDLER(
        uint16_t maxPacketSkip,
        ble::sync_timeout_t timeout,
        CommandResponsePtr& response
    )
    {
        ble_error_t err = gap().createSync(
            maxPacketSkip,
            timeout
        );
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(CancelCreateSync) {
    CMD_NAME("cancelCreateSync")
    CMD_HANDLER(CommandResponsePtr& response) {
        ble_error_t err = gap().cancelCreateSync();
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(TerminateSync) {
    CMD_NAME("terminateSync")
    CMD_HANDLER(ble::periodic_sync_handle_t handle, CommandResponsePtr& response) {
        ble_error_t err = gap().terminateSync(handle);
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(AddDeviceToPeriodicAdvertiserList) {
    CMD_NAME("addDeviceToPeriodicAdvertiserList")
    CMD_HANDLER(
        ble::peer_address_type_t::type peerAddressType,
        ble::address_t &peerAddress,
        ble::advertising_sid_t sid,
        CommandResponsePtr& response
    ) {
        ble_error_t err = gap().addDeviceToPeriodicAdvertiserList(
            peerAddressType,
            peerAddress,
            sid
        );
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(RemoveDeviceFromPeriodicAdvertiserList) {
    CMD_NAME("removeDeviceFromPeriodicAdvertiserList")
    CMD_HANDLER(
        ble::peer_address_type_t::type peerAddressType,
        ble::address_t &peerAddress,
        ble::advertising_sid_t sid,
        CommandResponsePtr& response
    ) {
        ble_error_t err = gap().removeDeviceFromPeriodicAdvertiserList(
            peerAddressType,
            peerAddress,
            sid
        );
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(ClearPeriodicAdvertiserList) {
    CMD_NAME("clearPeriodicAdvertiserList")
    CMD_HANDLER(CommandResponsePtr& response) {
        ble_error_t err = gap().clearPeriodicAdvertiserList();
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(GetMaxPeriodicAdvertiserListSize) {
    CMD_NAME("getMaxPeriodicAdvertiserListSize")
    CMD_HANDLER(CommandResponsePtr& response) {
        response->success(gap().getMaxPeriodicAdvertiserListSize());
    }
};

DECLARE_CMD(Connect) {
    CMD_NAME("connect")
    CMD_HANDLER(
        ble::peer_address_type_t::type peerAddressType,
        ble::address_t &peerAddress,
        CommandResponsePtr& response
    ) {
        ble_error_t err = gap().connect(peerAddressType, peerAddress, getConnectionParameters());
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(CancelConnect) {
    CMD_NAME("cancelConnect")
    CMD_HANDLER(CommandResponsePtr& response) {
        ble_error_t err = gap().cancelConnect();
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(UpdateConnectionParameters) {
    CMD_NAME("updateConnectionParameters")
    CMD_HANDLER(
        ble::connection_handle_t connectionHandle,
        ble::conn_interval_t minConnectionInterval,
        ble::conn_interval_t maxConnectionInterval,
        uint16_t slaveLatency,
        ble::supervision_timeout_t supervision_timeout,
        CommandResponsePtr& response
    ) {
        ble_error_t err = gap().updateConnectionParameters(
            connectionHandle,
            minConnectionInterval,
            maxConnectionInterval,
            slaveLatency,
            supervision_timeout
        );
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(ManageConnectionParametersUpdateRequest) {
    CMD_NAME("manageConnectionParametersUpdateRequest")
    CMD_HANDLER(bool manage, CommandResponsePtr& response) {
        ble_error_t err = gap().manageConnectionParametersUpdateRequest(manage);
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(AcceptConnectionParametersUpdate) {
    CMD_NAME("acceptConnectionParametersUpdate")
    CMD_HANDLER(
        ble::connection_handle_t connectionHandle,
        ble::conn_interval_t minConnectionInterval,
        ble::conn_interval_t maxConnectionInterval,
        uint16_t slaveLatency,
        ble::supervision_timeout_t supervision_timeout,
        CommandResponsePtr& response
    ) {
        ble_error_t err = gap().acceptConnectionParametersUpdate(
            connectionHandle,
            minConnectionInterval,
            maxConnectionInterval,
            slaveLatency,
            supervision_timeout
        );
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(RejectConnectionParametersUpdate) {
    CMD_NAME("rejectConnectionParametersUpdate")
    CMD_HANDLER(ble::connection_handle_t handle, CommandResponsePtr& response) {
        ble_error_t err = gap().rejectConnectionParametersUpdate(handle);
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(Disconnect) {
    CMD_NAME("disconnect")
    CMD_HANDLER(
        ble::connection_handle_t handle,
        ble::local_disconnection_reason_t::type reason,
        CommandResponsePtr& response
    ) {
        ble_error_t err = gap().disconnect(handle, reason);
        reportErrorOrSuccess(response, err);
    }
};

DECLARE_CMD(IsFeatureSupported) {
    CMD_NAME("isFeatureSupported")
    CMD_HANDLER(ble::controller_supported_features_t::type feature, CommandResponsePtr& response) {
        response->success(gap().isFeatureSupported(feature));
    }
};

} // end of annonymous namespace


DECLARE_SUITE_COMMANDS(GapV2CommandSuiteDescription,
    CMD_INSTANCE(GetMaxAdvertisingSetNumber),
    CMD_INSTANCE(GetMaxAdvertisingDataLength),
    CMD_INSTANCE(CreateAdvertisingSet),
    CMD_INSTANCE(DestroyAdvertisingSet),
    CMD_INSTANCE(SetAdvertisingParameters),
    CMD_INSTANCE(SetAdvertisingPayload),
    CMD_INSTANCE(SetAdvertisingScanResponse),
    CMD_INSTANCE(StartAdvertising),
    CMD_INSTANCE(StopAdvertising),
    CMD_INSTANCE(IsAdvertisingActive),
    CMD_INSTANCE(SetPeriodicAdvertisingParameters),
    CMD_INSTANCE(SetPeriodicAdvertisingPayload),
    CMD_INSTANCE(StartPeriodicAdvertising),
    CMD_INSTANCE(StopPeriodicAdvertising),
    CMD_INSTANCE(IsPeriodicAdvertisingActive),
    CMD_INSTANCE(SetScanParameters),
    CMD_INSTANCE(StartScan),
    CMD_INSTANCE(StopScan),
    CMD_INSTANCE(CreateSync),
    CMD_INSTANCE(CreateSyncFromList),
    CMD_INSTANCE(CancelCreateSync),
    CMD_INSTANCE(TerminateSync),
    CMD_INSTANCE(AddDeviceToPeriodicAdvertiserList),
    CMD_INSTANCE(RemoveDeviceFromPeriodicAdvertiserList),
    CMD_INSTANCE(ClearPeriodicAdvertiserList),
    CMD_INSTANCE(GetMaxPeriodicAdvertiserListSize),
    CMD_INSTANCE(Connect),
    CMD_INSTANCE(CancelConnect),
    CMD_INSTANCE(UpdateConnectionParameters),
    CMD_INSTANCE(ManageConnectionParametersUpdateRequest),
    CMD_INSTANCE(AcceptConnectionParametersUpdate),
    CMD_INSTANCE(RejectConnectionParametersUpdate),
    CMD_INSTANCE(Disconnect),
    CMD_INSTANCE(IsFeatureSupported)
)

void GapV2CommandSuiteDescription::init()
{
    enable_event_handling();
}