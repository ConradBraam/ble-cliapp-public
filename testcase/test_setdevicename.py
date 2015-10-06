from lib.bench import Bench

class Testcase(Bench):
    def __init__(self):
        Bench.__init__(self,
                       name="test_setdevicename",
                       title = "",
                       status = "development",
                       purpose = "",
                       component=['ble'],
                       type="smoke", # allowed values: installation, compatibility, smoke, regression, acceptance, alpha, beta, destructive, performance
                       requirements={
                           "duts": {
                               '*': { #requirements for all nodes
                                    "count":2,
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
        
        self.command(2, "test HRM 1 setDeviceName Rohit-Device-2")
        self.command(2, "ifconfig").verifyMessage(['Rohit-Device-2'])

    def rampDown(self):
        # nothing for now
        pass