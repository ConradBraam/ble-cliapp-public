#ifndef _BLE_MACROS_H
#define _BLE_MACROS_H
/**
 * Assertion and check macros
 */
 
/**
 * Execute a command (from BLE_API) and report failure. Note that there is a
 * premature return in the case of a failure.
 *
 * @param[in] CMD
 *                The command (function-call) to be invoked.
 */
#define ASSERT_NO_FAILURE(CMD) do {                        \
    ble_error_t error = (CMD);                             \
    if (error == BLE_ERROR_NONE) {                         \
        cmd_printf("{{success}}\r\n");                     \
    } else {                                               \
        cmd_printf("{{failure}} %s at line "               \
        "%u ERROR CODE: %u\r\n", #CMD, __LINE__, (error)); \
        return CMDLINE_RETCODE_FAIL;                       \
    }                                                      \
} while (0)
    
#define ASSERT_NO_FAILURE_VOID(CMD) do {                        \
    ble_error_t error = (CMD);                             \
    if (error == BLE_ERROR_NONE) {                         \
        cmd_printf("{{success}}\r\n");                     \
    } else {                                               \
        cmd_printf("{{failure}} %s at line "               \
        "%u ERROR CODE: %u\r\n", #CMD, __LINE__, (error)); \
    }                                                      \
} while (0)

#define CHECK_EQUALS(X,Y) ( \
                            (X) == (Y) ? \
                            cmd_printf("{{success}}\r\n") : \
                            cmd_printf("{{failure}}\r\n"));
                            
char* cmd_print_address(Gap::Address_t addr);
                            
#endif
