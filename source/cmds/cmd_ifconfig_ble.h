#ifndef _CMD_IFCONFIG_BLE_H
#define _CMD_IFCONFIG_BLE_H


#define MAN_IFCONFIG    "ifconfig (<iface>) ([options])\r\n"\
                    "<iface>                Interface, default: ble0\r\n"\
                    "up                     Initializa\r\n"\
                    "down                   Interface down\r\n"\
                    "reset                  Reset interface\r\n"\
                    "--set-dev-name <name>  Set Device Name\r\n"\
                    "--addr <addr>          Set Address\r\n"\
                    "--adv-timeout <t>      Set Advertising timeout\r\n"\
                    "--adv-interval <t>     Set Advertising interval\r\n"\
                    "start-adv              StartAdvertising\r\n"\
                    "clear-adv-payload      Clear Adv payload\r\n"\
                    "--acc-adv-tx-pwr <dBm> Set Acc adv tx power\r\n"\
                    "--acc-adv-payload <type> (<payload>)\r\n"\
                    "                       Set adv payload type and data\r\n"\
                    "                       Allowed types:\r\n"\
                    "                       LE_GENERAL_DISCOVERABLE\r\n"\
                    "                       BREDR_NOT_SUPPORTED\r\n"\
                    "                       SERVICE_DATA\r\n"\
                    "--set-appearance <app> Set Appearance\r\n"\
                    "                       Allowed values:\r\n"\
                    "                       GENERIC_PHONE"\
                    "--scan-params <interval>,<win>,<?>,<active scan>\r\n"\
                    "                       set scan-parameters\r\n"\
                    "--set-preferred-conn-params <a>,<b>,<c>,<d>\r\n"\
                    "                       Set Preferred Connection Parameters\r\n"\
                    "                       <a>: Minimum Connection Interval in 1.25 ms units\r\n"\
                    "                       <b>: Maximum Connection Interval in 1.25 ms units\r\n"\
                    "                       <c>: Slave Latency in number of connection events\r\n"\
                    "                       <d>: Connection Supervision Timeout in 10 ms units\r\n"\
                    "\r\n"\
                    "                       Without [options] just print current config\r\n"

BLE* get_ble_instance(void);
int cmd_ifconfig(int argc, char* argv[]);

#endif
