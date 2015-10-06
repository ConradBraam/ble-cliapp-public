from lib.bench import Bench

class Testcase(Bench):
    def __init__(self):
        Bench.__init__(self,
                       name="test_cmdline",
                       title = "Smoke test for command line interface",
                       status = "development",
                       purpose = "Verify Command Line Interface",
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

        #just demonstration purpose, probably not working...

        # just print all existing commands..
        self.command(1, "help")

        resp = self.command(1, "ifconfig")
        resp.verifyMessage(['ble0'])
        
        self.command('*', "ifconfig up")

    def rampDown(self):
        # nothing for now
        pass