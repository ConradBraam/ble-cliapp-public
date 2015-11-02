# Known issues

- The device can hang after of during a scan command: This issue will be addressed shortly, the current implementation use too much memory to write CommandResults to the command line.
- The device can easily run out of memory: The BLE API does not provide methods for unregistering callbacks wich can lead to memory leaks. This will be quickly fixed.