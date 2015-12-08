from BleTestCase import BleTestCase

class Testcase(BleTestCase):
    def __init__(self):
        BleTestCase.__init__(self,
                       name="test_advertising_interval",
                       title = "Advertising interval validation",
                       status = "released",
                       purpose = "Verify that a the advertising interval can be properly setup on a device",
                       component=['ble'],
                       feature=['advertising interval'],
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
        advertisingType = "ADV_CONNECTABLE_UNDIRECTED"

        # set up the type of advertising
        assertTrue( advertiser.gap("setAdvertisingType", advertisingType).success() )

        # add some data into advertising payload 
        assertTrue( advertiser.gap("accumulateAdvertisingPayload", "FLAGS", *advertiserFlags).success() )

        # give an arbitrary value for the advertising interval
        assertTrue( advertiser.gap("setAdvertisingInterval", str(100)).success() ) 

        # get the advertiser mac address
        resp = advertiser.gap("getAddress")
        assertTrue( resp.success() )
        advertiserAddress = resp.result['address']

        # test all advertising intervals between 100 and 1000 ms, the step is 100ms
        for advertisingInterval in range(1000, 2000, 500):
          # setup the advertising interval
          assertTrue( advertiser.gap("setAdvertisingInterval", str(advertisingInterval)).success() ) 

          # start advertising 
          assertTrue( advertiser.gap("startAdvertising").success() )

          # start scanning for 4x the time of the advertising interval
          resp = scanner.gap("startScan", str(4 * advertisingInterval), advertiserAddress)
          assertTrue( resp.success() )

          # check that the data are here 
          referenceTime = None
          for scan in resp.result:
            if referenceTime is None:
              referenceTime = scan['time']
            else:
              scanInterval = scan['time'] - referenceTime
              referenceTime = scan['time']
              assertTrue((scanInterval < (advertisingInterval + 10)) and (scanInterval > (advertisingInterval - 10)))

          # stop advertising 
          assertTrue( advertiser.gap("stopAdvertising").success() )


    def rampDown(self):
        # shutdown both interfaces 
        resp = self.advertiser.ble("shutdown")
        self.assertTrue(resp.success())

        resp = self.scanner.ble("shutdown")
        self.assertTrue(resp.success())
        pass