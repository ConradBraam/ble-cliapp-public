from lib.bench import Bench

class Testcase(Bench):
    def __init__(self):
        Bench.__init__(self,
                       name="test_cmdline",
                       title = "Smoke test for command line interface",
                       status = "smoke",
                       purpose = "Verify Command Line Interface",
                       type="smoke", # allowed values: installation, compatibility, smoke, regression, acceptance, alpha, beta, destructive, performance
                       requirements={
                           "duts": {
                               '*': { #requirements for all nodes
                                    "count":1,
                                    "type": "hardware",
                                    "application":{ "name":"generalTestApplication", "version": "1.0"}
                               }
                           }}
        )

    def rampUp(self):
        # nothing for now
        pass

    def case(self):
        resp = self.command(1, "ifconfig")
        
        res.verifyMessage(['ble0'])
        
        self.verifyTrace(1, ['cmd_ifconfig()'])
        self.verifyTrace(1, ['cmd_ifconfig_ble()'])
        self.verifyTrace(1, ['cmd_ifconfig_print()'])

    def rampDown(self):
        # nothing for now
        pass