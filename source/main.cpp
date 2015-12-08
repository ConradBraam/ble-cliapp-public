#include "mbed-drivers/mbed.h"
#include "ble/BLE.h"
#include "ble/services/iBeacon.h"
#include "mbed-client-cli/ns_cmdline.h"
#include "mbed-client-trace/mbed_client_trace.h"

#include "CLICommand/CommandSuite.h"
#include "Commands/BLECommands.h"
#include "Commands/GapCommands.h"

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


/** The queue used to hold received characters. */
#define UART_RX_LEN 256
uint8_t uart_rx_buffer[UART_RX_LEN];
uint16_t uart_rx_rd = 0;
uint16_t uart_rx_wr = 0;
int16_t uart_rx_get(void)
{
    int16_t rx_byte = -1;
    if (uart_rx_rd != uart_rx_wr)
    {
        uint16_t ptr = uart_rx_rd;
        rx_byte = uart_rx_buffer[ptr++];
        if (ptr >= UART_RX_LEN) ptr = 0;
        uart_rx_rd = ptr;
    }
    return rx_byte;

}
// serial RX interrupt function
// there should be buffer to improve performance..
void minarCallback(void) 
{
    int16_t rx;
    do {
        rx = uart_rx_get();
        if( rx >= 0 ) {
            cmd_char_input(rx);
        }
    } while(rx);
}
void rx_interrupt(void)
{
    char chr = pc.getc();
    uint16_t index;
    bool sendEvent = (uart_rx_rd == uart_rx_wr);
    index = uart_rx_wr;
    index++;
    if (index >= UART_RX_LEN) index = 0;
    if (index != uart_rx_rd)
    {
            uart_rx_buffer[uart_rx_wr] = chr;
            uart_rx_wr = index;
    }
    if(sendEvent)
    {
        minar::Scheduler::postCallback(minarCallback);
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
    registerCommandSuite<BLECommandSuiteDescription>();
    registerCommandSuite<GapCommandSuiteDescription>();
}

void app_start(int, char*[])
{
    //configure serial port
    pc.baud(115200);	// This is default baudrate for our test applications. 230400 is also working, but not 460800. At least with k64f.
    pc.attach(&rx_interrupt);
    
    
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
