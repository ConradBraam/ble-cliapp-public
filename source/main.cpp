#ifdef YOTTA_CFG
#include "mbed-drivers/mbed.h"
#else
#include "mbed.h"
#endif
#include "ble/BLE.h"
#include "ble/services/iBeacon.h"
#include "mbed-client-cli/ns_cmdline.h"
#include "mbed-trace/mbed_trace.h"

#include "CLICommand/CommandEventQueue.h"
#include "CLICommand/CommandSuite.h"
#include "Commands/BLECommands.h"
#include "Commands/GapCommands.h"
#include "Commands/GattServerCommands.h"
#include "Commands/GattClientCommands.h"
#include "Commands/SecurityManagerCommands.h"
#include "Commands/parameters/AdvertisingParameters.h"
#include "Commands/parameters/ScanParameters.h"

#include "util/CriticalSectionLock.h"
typedef ::util::CriticalSectionLock CriticalSection;

#include "util/CircularBuffer.h"


#ifdef YOTTA_CFG
#include "EventQueue/EventQueueMinar.h"

static eq::EventQueueMinar _taskQueue;
#else
#include "EventQueue/EventQueueClassic.h"

static eq::EventQueueClassic<10> _taskQueue;
#endif

/**
 * Macros for setting console flow control.
 */
#define CONSOLE_FLOWCONTROL_RTS     1
#define CONSOLE_FLOWCONTROL_CTS     2
#define CONSOLE_FLOWCONTROL_RTSCTS  3
#define mbed_console_concat_(x) CONSOLE_FLOWCONTROL_##x
#define mbed_console_concat(x) mbed_console_concat_(x)
#define CONSOLE_FLOWCONTROL mbed_console_concat(MBED_CONF_TARGET_CONSOLE_UART_FLOW_CONTROL)

eq::EventQueue& taskQueue = _taskQueue;

// Prototypes
void cmd_ready_cb(int retcode);
static void whenRxInterrupt(void);
static void consumeSerialBytes(void);

RawSerial& get_serial() {
    static RawSerial serial(USBTX, USBRX);
    static bool initialized = false;

    if (!initialized) {
#if   CONSOLE_FLOWCONTROL == CONSOLE_FLOWCONTROL_RTS
    serial.set_flow_control(SerialBase::RTS, STDIO_UART_RTS, NC);
#elif CONSOLE_FLOWCONTROL == CONSOLE_FLOWCONTROL_CTS
    serial.set_flow_control(SerialBase::CTS, NC, STDIO_UART_CTS);
#elif CONSOLE_FLOWCONTROL == CONSOLE_FLOWCONTROL_RTSCTS
    serial.set_flow_control(SerialBase::RTSCTS, STDIO_UART_RTS, STDIO_UART_CTS);
#endif
        initialized = true;
    }

    return serial;
}
// constants
static const size_t CIRCULAR_BUFFER_LENGTH = 768;
static const size_t CONSUMER_BUFFER_LENGTH = 32;

// circular buffer used by serial port interrupt to store characters
// It will be use in a single producer, single consumer setup:
// producer => RX interrupt
// consumer => a callback run by minar
static ::util::CircularBuffer<uint8_t, CIRCULAR_BUFFER_LENGTH> rxBuffer;

// callback called when a character arrive on the serial port
// this function will run in handler mode
static void whenRxInterrupt(void)
{
    static RawSerial& serial = get_serial();

    if (serial.readable()) {
        bool startConsumer = rxBuffer.empty();

        while (serial.readable()) {
            if(rxBuffer.push((uint8_t) serial.getc()) == false) {
                error("error, serial buffer is full\r\n");
            }
        }

        if(startConsumer) {
            taskQueue.post(consumeSerialBytes);
        }
    }
}

// consumptions of bytes from the serial port.
// this function should run in thread mode
static void consumeSerialBytes(void) {
// buffer of data
    uint8_t data[CONSUMER_BUFFER_LENGTH];
    uint32_t dataAvailable = 0;
    bool shouldExit = false;
    do {
        {
            CriticalSection lock;
            dataAvailable = rxBuffer.pop(data);
            if(!dataAvailable) {
                // sanity check, this should never happen
                error("error, serial buffer is empty\r\n");
            }
            shouldExit = rxBuffer.empty();
        }

        std::for_each(data, data + dataAvailable, cmd_char_input);
    } while(shouldExit == false);
}

void custom_cmd_response_out(const char* fmt, va_list ap)
{
    // ARMCC microlib does not properly handle a size of 0.
    // As a workaround supply a dummy buffer with a size of 1.
    char dummy_buf[1];
    int len = vsnprintf(dummy_buf, sizeof(dummy_buf), fmt, ap);
    if (len < 100) {
        char temp[100];
        vsprintf(temp, fmt, ap);
        get_serial().puts(temp);
    } else {
        char *temp = new char[len + 1];
        vsprintf(temp, fmt, ap);
        get_serial().puts(temp);
        delete[] temp;
    }
}

// this function should be inside some "event scheduler", because
// then cli can be use also with async commands
// see example with event-loop (/minar): https://github.com/ARMmbed/mbed-client-cliapp/blob/master/source/cmd_commands.c
void cmd_ready_cb(int retcode)
{
    cmd_next( retcode );
}

void initialize_app_commands(void) {
    // setup the event queue for the CLICommand module
    initCLICommandEventQueue(&taskQueue);

    // register command suite in the system
    registerCommandSuite<BLECommandSuiteDescription>();
    registerCommandSuite<GapCommandSuiteDescription>();
    registerCommandSuite<GattServerCommandSuiteDescription>();
    registerCommandSuite<GattClientCommandSuiteDescription>();
    registerCommandSuite<SecurityManagerCommandSuiteDescription>();
    registerCommandSuite<AdvertisingParametersCommandSuiteDescription>();
    registerCommandSuite<ScanParametersCommandSuiteDescription>();
}

void scheduleBleEventsProcessing(BLE::OnEventsToProcessCallbackContext* context) {
    taskQueue.post(&BLE::processEvents, &context->ble);
}

void app_start(int, char*[])
{
    BLE& ble = BLE::Instance(BLE::DEFAULT_INSTANCE);
    ble.onEventsToProcess(scheduleBleEventsProcessing);

    //configure serial port
    get_serial().baud(115200);    // This is default baudrate for our test applications. 230400 is also working, but not 460800. At least with k64f.
    get_serial().attach(whenRxInterrupt);

    cmd_init( &custom_cmd_response_out );
    cmd_set_ready_cb( cmd_ready_cb );
    cmd_history_size(1);
    initialize_app_commands();
}

#ifndef YOTTA_CFG
int main(void)
{
    app_start(0, NULL);

    while (true) {
        _taskQueue.dispatch();
        // TODO: should wait for event/ go to sleep, even if BLE_API is not active ...
        //ble.waitForEvent(); // this will return upon any system event (such as an interrupt or a ticker wakeup)
    }
}
#endif

// Custom implementation for mbed_die,
// this reduce the memory consumption
extern "C" void mbed_die(void) {
    while(true) { }
}
