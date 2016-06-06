#include "mbed-drivers/mbed.h"
#include "ble/BLE.h"
#include "ble/services/iBeacon.h"
#include "mbed-client-cli/ns_cmdline.h"
#include "mbed-trace/mbed_trace.h"

#include "CLICommand/CommandSuite.h"
#include "Commands/BLECommands.h"
#include "Commands/GapCommands.h"
#include "Commands/GattServerCommands.h"
#include "Commands/GattClientCommands.h"
#include "Commands/SecurityManagerCommands.h"

// mbed::util::CriticalSectionLock is broken on nordic platform.
// This is a temporary workaround which should be removed as soon as
// https://github.com/ARMmbed/core-util/pull/50 is accepted and published
// in yotta registry.
#ifdef TARGET_NORDIC
#include "util/NordicCriticalSectionLock.h"
typedef ::util::NordicCriticalSectionLock CriticalSection;
#else
#include <core-util/CriticalSectionLock.h>
typedef ::mbed::util::CriticalSectionLock CriticalSection;
#endif

#include "util/CircularBuffer.h"

// Prototypes
void cmd_ready_cb(int retcode);
static void whenRxInterrupt(void);
static void consumeSerialBytes(void);

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
    bool startConsumer = rxBuffer.empty();
    if(rxBuffer.push((uint8_t) get_stdio_serial().getc()) == false) {
        error("error, serial buffer is full\r\n");
    }

    if(startConsumer) {
        minar::Scheduler::postCallback(consumeSerialBytes);
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

void trace_printer(const char* str)
{
	get_stdio_serial().printf("%s\r\n", str);
	cmd_output();
}

void custom_cmd_response_out(const char* fmt, va_list ap)
{
    vprintf(fmt, ap);
    fflush(stdout);
}

// this function should be inside some "event scheduler", because
// then cli can be use also with async commands
// see example with event-loop (/minar): https://github.com/ARMmbed/mbed-client-cliapp/blob/master/source/cmd_commands.c
void cmd_ready_cb(int retcode)
{
    cmd_next( retcode );
}

void initialize_app_commands(void) {
    registerCommandSuite<BLECommandSuiteDescription>();
    registerCommandSuite<GapCommandSuiteDescription>();
    registerCommandSuite<GattServerCommandSuiteDescription>();
    registerCommandSuite<GattClientCommandSuiteDescription>();
    registerCommandSuite<SecurityManagerCommandSuiteDescription>();
}

void app_start(int, char*[])
{
    //configure serial port
    get_stdio_serial().baud(115200);	// This is default baudrate for our test applications. 230400 is also working, but not 460800. At least with k64f.
    get_stdio_serial().attach(whenRxInterrupt);

    // initialize trace libary
    mbed_trace_init();
    mbed_trace_print_function_set( trace_printer );
    mbed_trace_cmdprint_function_set( cmd_printer );
    mbed_trace_config_set(TRACE_MODE_COLOR|TRACE_ACTIVE_LEVEL_DEBUG|TRACE_CARRIAGE_RETURN);

    cmd_init( &custom_cmd_response_out );
    cmd_set_ready_cb( cmd_ready_cb );
    initialize_app_commands();
}
/**
 * main() is needed only for mbed-classic. mbed OS triggers app_start() automatically.
 */
#ifndef YOTTA_CFG_MBED_OS
int main(void)
{
    app_start(0, NULL);

    return 0;
}
#endif
