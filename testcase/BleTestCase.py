from lib.bench import Bench

import json

class CommandResult:
  def __init__(self, status, error, result):
    self.status = status
    self.error = error
    self.result = result

  @classmethod
  def fromJSON(cls, obj):
    return cls(obj['status'], obj.get('error'), obj.get('result'))

  def success(self):
    return self.status == 0


class BleDevice:
    def __init__(self, tc, deviceId):
      self.tc = tc
      self.id = deviceId

    def command(self, module, cmd, *argv):
        command = module + " " + cmd + " "
        if argv is not None:
          command += " ".join(argv)

        cmdResponse = self.tc.command(self.id, command)
        # remove the retcode ...
        cmdResponse.lines.pop()
        return CommandResult.fromJSON(json.loads("".join(cmdResponse.lines)))

    def ble(self, cmd, *argv):
      return self.command("ble", cmd, *argv)

    def gap(self, cmd, *argv):
      return self.command("gap", cmd, *argv)


class BleTestCase(Bench):
    def __init__(self, **kwargs):
        Bench.__init__(self, **kwargs)

    def getDevice(self, id):
      return BleDevice(self, id)
