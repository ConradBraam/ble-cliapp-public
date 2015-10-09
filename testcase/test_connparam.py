from lib.bench import Bench

class Testcase(Bench):
    def __init__(self):
        Bench.__init__(self,
                       name="test_connparam",
                       title = "Set up connection parameters tuple and revert to original connection settings",
                       status = "released",
                       purpose = "Verify that connection parameters can be changed",
                       component=['ble'],
                       feature=['ifconfig'],
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

        # Connection parameters tuple (min conn, max conn, latency, timeout)
        connParams = [
            [50, 500, 0, 500],
            [40, 400, 0, 400],
            [50, 400, 0, 500],
        ]

        for conn in connParams:
            conn_str = ','.join([str(s) for s in conn])
            resp = self.command(1, "test HRM 1 connParam %s"% conn_str)
            resp.verifyMessage(['minConnectionInterval: %d'% conn[0]])
            resp.verifyMessage(['maxConnectionInterval: %d'% conn[1]])
            resp.verifyMessage(['slaveLatency: %d'% conn[2]])
            resp.verifyMessage(['connectionSupervisionTimeout: %d'% conn[3]])

    def rampDown(self):
        # nothing for now
        pass
