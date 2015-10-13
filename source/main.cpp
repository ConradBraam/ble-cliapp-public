#include "mbed.h"
#include "ble/BLE.h"
#include "ble/services/iBeacon.h"
#include "mbed-client-cli/ns_cmdline.h"
#define YOTTA_CFG_MBED_CLIENT_TRACE
#include "mbed-client-trace/mbed_client_trace.h"

#include "cmds/connect.h"

// Prototypes
void cmd_ready_cb(int retcode);

Serial pc(USBTX, USBRX);

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

// serial RX interrupt function
// there should be buffer to improve performance..
void cmd_cb(void)
{
    cmd_char_input(pc.getc());
}

// this function should be inside some "event scheduler", because
// then cli can be use also with async commands
// see example with event-loop (/minar): https://github.com/ARMmbed/mbed-client-cliapp/blob/master/source/cmd_commands.c
void cmd_ready_cb(int retcode)
{
    cmd_next( retcode );
}

void app_start(int, char*[])
{
    //configure serial port
    pc.baud(115200);    // This is default baudrate for our test applications. 230400 is also working, but not 460800. At least with k64f.
    pc.attach(&cmd_cb);

    // initialize trace libary
    mbed_client_trace_init();
    mbed_client_trace_print_function_set( trace_printer );
    mbed_client_trace_cmdprint_function_set( trace_printer );
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
