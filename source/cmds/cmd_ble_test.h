#ifndef _CMD_BLE_TEST_H
#define _CMD_BLE_TEST_H

#define MAN_TEST    "test <id> <node> ([options]) Execute individual test\r\n"\
                    "<id>                   TC ID. Allowed values:  'A', 'HRM', 'BlockTransfer' ..\r\n"\
                    "<node>                 Node ID. Allowed values:  '1','2'\r\n"\
                    "<options>              e.g. subtest for A:1\r\n"\
                    "                       shutdownTest\r\n"\
                    "                       setTimeoutTest\r\n"\
                    "                       changePayloadTest\r\n"\
                    "                       changeIntervalTest\r\n"\
                    "                       setAddrTest\r\n"\
                    "                       setupIBeaconTest\r\n"

void init_ble_test(BLE* ble);
int cmd_test(int argc, char* argv[]);

#endif
