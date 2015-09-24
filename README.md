# ble command line application

# folder structure
* source
 device side application source files
* testcase
 clitest -based example test case, which (should) work with device application

run: 
```
git clone https://github.com/ARMmbed/ble-cliapp
cd ble-cliapp
yt build
```
...hope that everything success...
-> flash device and reboot

```
git clone https://github.com/ARMmbed/mbed-clitest
copy testcase/test_cmdline.py mbed-clitest/testcases
cd mbed-clitest
python clitest.py --tc test_cmdline -v
```

notes:
See help page:
```python clitest.py --help```

device reset not working yet with mbed -devices
```clitest.py --reset```

probably tests could be moved to own repository, but it is not mandatory.
You can select which directly contains tests by --tcdir -parameter:
```clitest --tcdir <tc-directory>```

see more examples tests from clitest -repository example folder:
https://github.com/ARMmbed/mbed-clitest/tree/master/examples


