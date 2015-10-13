from lib.bench import Bench

class Testcase(Bench):
    def __init__(self):
        Bench.__init__(self,
                       name="test_setdevicename",
                       title = "Set BLE device name",
                       status = "released",
                       purpose = "Verify BLE device name set/get functionality",
                       component=['ble'],
                       feature=['iconfig'],
                       type="regression", # allowed values: installation, compatibility, smoke, regression, acceptance, alpha, beta, destructive, performance
                       requirements={
                           "duts": {
                               '*': { #requirements for all nodes
                                    "count":1,
                                    "type": "hardware",
                                    "application":{ "name":"generalTestApplication", "version": "1.0"},
                               }
                           }}
        )

    def rampUp(self):
        # nothing for now
        pass

    def case(self):

        # Device Name configuration

        self.command(1, "test HRM 1 setDeviceName Rohit-Device-1")
        self.command(1, "ifconfig").verifyMessage(['Rohit-Device-1'])

    def rampDown(self):
        # nothing for now
        pass
