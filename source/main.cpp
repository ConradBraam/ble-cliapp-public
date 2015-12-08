#include "mbed-drivers/mbed.h"
#include "ble/BLE.h"
#include "ble/services/iBeacon.h"
#include "mbed-client-cli/ns_cmdline.h"
#include "mbed-client-trace/mbed_client_trace.h"

#include "CLICommand/CommandSuite.h"
#include "Commands/BLECommands.h"
#include "Commands/GapCommands.h"
#include "Commands/GattServerCommands.h"
#include "Commands/GattClientCommands.h"

#include <core-util/atomic_ops.h>
#include <mbed-drivers/CircularBuffer.h>

// Prototypes
void cmd_ready_cb(int retcode);

static Serial pc(USBTX, USBRX);

static const size_t CIRCULAR_BUFFER_LENGTH = 128;

// circular buffer used by serial port interrupt to store characters
// It will be use in a single producer, single consumer setup:
// producer => RX interrupt
// consumer => a callback run by yotta
// note: This class is not designed for this kind of setup, we try to mitigate
// this by relyin on an external counter instead of an internal one
static CircularBuffer<uint8_t, CIRCULAR_BUFFER_LENGTH> rxBuffer;

// a counter is used to track count of bytes not yet consumed
static uint32_t bytesNotConsumed = 0;

// callback called when a character arrive on the serial port
void whenRxInterrupt(void)
{
    char chr = pc.getc();
    rxBuffer.push((uint8_t) chr);

    if(mbed::util::atomic_incr(&bytesNotConsumed, (uint32_t) 1) == 1) {
        // if it is the first character send, just post a callback into minar
        minar::Scheduler::postCallback([]() {
            do {
                uint8_t data = 0;
                if(rxBuffer.pop(data) == false) {
                    error("invalid state of rxBuffer");
                }
                cmd_char_input(data);
            } while(mbed::util::atomic_decr(&bytesNotConsumed,  (uint32_t) 1));
        });
    }
}

void trace_printer(const char* str)
{
	pc.printf("%s\r\n", str);
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
}

void app_start(int, char*[])
{
    //configure serial port
    pc.baud(115200);	// This is default baudrate for our test applications. 230400 is also working, but not 460800. At least with k64f.
    pc.attach(whenRxInterrupt);


    // initialize trace libary
    mbed_client_trace_init();
    mbed_client_trace_print_function_set( trace_printer );
    mbed_client_trace_cmdprint_function_set( cmd_printer );
    mbed_client_trace_config_set(TRACE_MODE_COLOR|TRACE_ACTIVE_LEVEL_DEBUG|TRACE_CARRIAGE_RETURN);

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
