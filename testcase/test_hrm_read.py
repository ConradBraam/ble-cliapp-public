from lib.bench import Bench

class Testcase(Bench):
    def __init__(self):
        Bench.__init__(self,
                       name="test_hrm_read",
                       title = "HRM characteristic read operation",
                       status = "broken",
                       purpose = "Verify HRM characteristic reading",
                       component=['ble'],
                       feature=['HRM'],
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

    def rampUp(self):
        # nothing for now
        pass

    def case(self):

        # Connection

        # Set up Node '1'
        self.command(1, "ifconfig").verifyMessage(['ble0'])
        self.command(1, "ifconfig up")
        self.command(1, "test HRM 1 setDeviceName HRM-Device")

        # Set up Node '2'
        self.command(2, "ifconfig").verifyMessage(['ble0'])
        self.command(2, "ifconfig up")

        # Get MAC address of Node 1 and make Node 2 connect to node 1
        dut1_public_address = self.get_ble0_address(1, type='random-static')

        # Conenction....
        self.command(2, "test HRM 2 connect %s"% dut1_public_address)

        self.command(2, "test HRM 2 read")

        # Disconenction....
        self.command(2, "test HRM 2 disconnect")

    def rampDown(self):
        # nothing for now
        pass
