from lib.bench import Bench

class Testcase(Bench):
    def __init__(self):
        Bench.__init__(self,
                       name="test_appearance",
                       title = "",
                       status = "development",
                       purpose = "",
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

        # Appearance related test cases

        self.command(1, "help")

        self.command(1, "ifconfig").verifyMessage(['ble0'])

        self.command(1, "ifconfig up")
        self.command(2, "ifconfig up")

        self.command(1, "test HRM 1 appearance 64")
        self.command(1, "ifconfig").verifyMessage(['Generic Phone'])
        
        self.command(2, "test HRM 1 appearance 64")
        self.command(2, "ifconfig").verifyMessage(['Generic Phone'])
        
    def rampDown(self):
        # nothing for now
        pass