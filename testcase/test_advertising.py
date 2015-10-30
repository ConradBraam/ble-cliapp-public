from BleTestCase import BleTestCase

class Testcase(BleTestCase):
    def __init__(self):
        BleTestCase.__init__(self,
                       name="test_advertising",
                       title = "Advertising check",
                       status = "released",
                       purpose = "Verify that a device can advertise",
                       component=['ble'],
                       feature=['advertising'],
                       type="regression", # allowed values: installation, compatibility, smoke, regression, acceptance, alpha, beta, destructive, performance
                       requirements={
                           "duts": {
                               '*': { #requirements for all nodes
                                    "count":2,
                                    "type": "hardware",
                                    "application":{ "name":"generalTestApplication", "version": "1.0"},
                               }
                           }}
        )
        # two devices are under test, one which advertise and the other which scan
        self.advertiser = self.getDevice(1)
        self.scanner = self.getDevice(2)

    def rampUp(self):
        # reset both devices
        self.assertTrue( self.advertiser.ble("init").success() )
        self.assertTrue( self.scanner.ble("init").success() )

        # clear advertising payload of the advertiser 
        self.assertTrue( self.advertiser.gap("clearAdvertisingPayload").success() )

    def case(self):
        advertiser = self.advertiser
        scanner = self.scanner
        assertTrue = lambda X: self.assertTrue(X)


        advertiserFlags = ["LE_GENERAL_DISCOVERABLE", "BREDR_NOT_SUPPORTED"]
        advertiserName = "fooDevice"
        advertisingInterval = 1000
        advertisingType = "ADV_CONNECTABLE_UNDIRECTED"

        # set up the type of advertising
        assertTrue( advertiser.gap("setAdvertisingType", advertisingType).success() )

        # add some data into advertising payload 
        assertTrue( advertiser.gap("accumulateAdvertisingPayload", "FLAGS", *advertiserFlags).success() )
        assertTrue( advertiser.gap("accumulateAdvertisingPayload", "COMPLETE_LOCAL_NAME", advertiserName).success() )

        # get the advertiser mac address
        resp = advertiser.gap("getAddress")
        assertTrue( resp.success() )
        advertiserAddress = resp.result['address']

        # setup the advertising interval
        assertTrue( advertiser.gap("setAdvertisingInterval", str(advertisingInterval)).success() ) 

        # start advertising 
        assertTrue( advertiser.gap("startAdvertising").success() )

        # start scanning for 3 seconds
        resp = scanner.gap("startScan", '3000', advertiserAddress)
        assertTrue( resp.success() )

        # check that the data are here 
        for scan in resp.result:
          assertTrue( scan['data']['COMPLETE_LOCAL_NAME'] == advertiserName )
          assertTrue( scan['data']['FLAGS'] == advertiserFlags )
          assertTrue( scan['peerAddr'] == advertiserAddress) 
          assertTrue( scan['type'] == "ADV_CONNECTABLE_UNDIRECTED")

    def rampDown(self):
        # shutdown both interfaces 
        resp = self.advertiser.ble("shutdown")
        self.assertTrue(resp.success())

        resp = self.scanner.ble("shutdown")
        self.assertTrue(resp.success())
        pass