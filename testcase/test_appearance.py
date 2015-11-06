from mbed_clitest.bench import Bench

class Testcase(Bench):
    def __init__(self):
        Bench.__init__(self,
                       name="test_appearance",
                       title = "Change BLE device appearance",
                       status = "released",
                       purpose = "Check if we can change BLE appearance to BLE_APPEARANCE_GENERIC_PHONE",
                       component=['ble'],
                       feature=['appearance'],
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

        BLE_APPEARANCE_GENERIC_PHONE = 64

        # Appearance related test cases
        resp = self.command(1, "ifconfig")
        resp.verifyMessage(['ble0'])
        self.command(1, "ifconfig up")
        self.command(1, "test HRM 1 appearance %d"% BLE_APPEARANCE_GENERIC_PHONE)
        resp = self.command(1, "ifconfig")
        resp.verifyMessage(['Generic Phone'])

    def rampDown(self):
        # nothing for now
        pass
