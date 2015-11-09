from BleTestCase import BleTestCase

class Testcase(BleTestCase):
    def __init__(self):
        BleTestCase.__init__(self,
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
        self.dut = self.getDevice(1)

    def rampUp(self):
        resp = self.dut.ble("reset")
        self.assertTrue(resp.success())
        # nothing for now
        pass

    def case(self):
        dut = self.dut
        deviceAppearance = "GENERIC_HEART_RATE_SENSOR"

        resp = dut.gap("setAppearance", deviceAppearance)
        self.assertTrue(resp.status == 0)

        resp = dut.gap("getAppearance")
        self.assertTrue(resp.status == 0) 
        self.assertTrue(resp.result == deviceAppearance) 

    def rampDown(self):
        resp = self.dut.ble("shutdown")
        self.assertTrue(resp.success())
        pass
