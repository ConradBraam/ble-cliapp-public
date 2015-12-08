# ble command line application

This application provide a shell to exercise mbed os BLE API. 
Basically the user send command to the device through the UART, then the device try to execute the command and return the result of the operation. 

There is two main use case for this application:
  * Exercise manualy BLE API through a terminal
  * Run and write tests over the BLE API by using [mbed-clitest] (https://github.com/ARMmbed/mbed-clitest)


## Getting started 

You will need several things to start with this application:
  * A board supporting mbed os BLE API like the [nrf51dk](https://developer.mbed.org/platforms/Nordic-nRF51-DK/)
  * [Yotta](http://yottadocs.mbed.com/#installing) installed and configured on your computer
  * A terminal program

First, you will have to build this program:
  * Clone this repository: `git clone https://github.com/ARMmbed/ble-cliapp`
  * Select your target, for the nrf51dk and the GCC compiler: `yotta target nrf51dk-gcc`
  * Build: `yotta build`

If the build succeed, your programm is now available in your working directory at: `build/<target-name>/source/ble-cliapp(-xxx).hex`
Load the program into your board then start your terminal.

Now you can play start to play with the shell !


## Shell organization 

### Modules
The shell is organized into modules which contains commands. The format to call a command is 

```<module> <command> [command args]```


### Builtin commands 
Every modules come with a set of builtin commands. These commands will provide you help about the 
module and command of the module:
  * `<module> list`: This command list all the commands available in the module. It is the entry point
                     if you want to start to discover the functionality of a module
  * `<module> help <command_name>`: This command provide help to a specific command of a module
  * `<module> args <command_name>`: This command provide information about arguments taken by a command of a module


### Existing module
At the moment, only two modules are supported:
  * `ble`: Which map the functions (not the deprecated ones) contained in ble/BLE.h 
  * `gap`: Which map the functions contained in ble/Gap.h 


## Command/Response format

**Warning: This part of ble-cliapp is still under heavy development and things will be broke without notice in the near future.**

This application use a well defined format for every command and every response.


### Commands 

The client use a command line/ a terminal to send its command to the ble RCP server. The format of this command is:

```bash
<module> <comand> <args>
``` 

  * `module` The module owning the command 
  * `command` The command to invoke
  * `args` The arguments of the command, each arguments are separated by at least a space


### Response

The server respond to each command. The response is a JSON document where each response share the following base:

``` javascript
{
  'name': '<module> <command>',   /* the name of the command invoked wich have produced this result */
  'arguments': [ 
    'list', 
    'of'
    'arguments'
  ].                              /* array of arguments used to invoke the command */
  'status': <status code>         /* if eqal to 0 then it is a success, otherwise it is an error */
}
```


#### Error 

In case of error, the JSON doccument can also contain the field `error` which provide details about the error.

``` javascript
{
  'name': '<module> <command>',   /* the name of the command invoked wich have produced this result */
  'arguments': [ 
    'list', 
    'of'
    'arguments'
  ].                              /* array of arguments used to invoke the command */
  'status': <status code>,        /* if eqal to 0 then it is a success, otherwise it is an error */
  'error': <JSON value>           /* error details */
}
```


#### Success 

In case of success, the JSON doccument can also contain the field `result` which provide the result of the command.

``` javascript
{
  'name': '<module> <command>',   /* the name of the command invoked wich have produced this result */
  'arguments': [ 
    'list', 
    'of'
    'arguments'
  ].                              /* array of arguments used to invoke the command */
  'status': <status code>,        /* if eqal to 0 then it is a success, otherwise it is an error */
  'result': <JSON value>          /* result of the operation */
}
```



## Scripting and testing 

it is possible to use this shell to automate testing on mbed os BLE devices. Tests are built with 
[mbed-clitest](https://github.com/ARMmbed/mbed-clitest).

### Getting started 

```bash 
git clone https://github.com/ARMmbed/mbed-clitest
cd mbed-clitest
git submodule init
git submodule update
# All tests should be copied into mbed-clitest/testcases
copy ../ble-cliapp/testcase/test_cmdline.py mbed-clitest/testcases
```

### Running tests

Go into the folder containing mbed-clitest then you can start running tests:

```python clitest.py --tc <test_name> -v```

If you want more informations, read the help of mbed-clitest

```python clitest.py --help```


### Writing more tests 

The easiest way to add tests is by extending the BleTestCase class. It provide simple primitives to 
deal with the command and response format and built easy to use device abstractions.

#### BleTestCase API 

  * `BleTestCase.getDevice(self, id) -> BleDevice` : Return an instance of the device referenced by `id`.
  * `BleDevice`: Object representing a device running mbed os BLE shell
    - `BleDevice.command(self, cmd, expectedRetcode = 0) -> CommandResult` : Execute a command. The format is 
      the same as the one used in command line. However, in this case cmd is a string specifying the module,
      command and arguments. Furthermore, if the command is expected to fail, then `expectedRetcode` can be set
      to the expected failure value, otherwise mbed-clitest will stop the test.
    - `BleDevice.ble(self, cmd, *argv) -> CommandResult` : Shortcut for `BleDevice.command(self, 'ble' + cmd + *argv)`. 
    - `BleDevice.bleWithRetcode(self, expectedRetcode, cmd, argv*) -> CommandResult` : Shortcut for
      `BleDevice.command(self, 'ble' + cmd + *argv, expectedRetcode)`.
    - `BleDevice.gap(self, cmd, *argv) -> CommandResult` : Shortcut for `BleDevice.command(self, 'gap' + cmd + *argv)`. 
  * `CommandResult`: This is the result of the command, this class contains three attributes 
    - `status`: The result status code
    - `error`: Error informations about this command, this attribute is optionnal. The format of this attribute is 
      the same as the one returned in the JSON document.
    - `result`: Result of this command, this attribute is optional. The format of this attribute is 
      the same as the one returned in the JSON document.
    - CommandResult.success(self) -> boolean : indicate if the operation have succeeded or not.


#### Full example 

```python
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
```



## Extend command and modules: 

It is possible to add more modules and more commands to BLE cliapp. 


### Commands: 

It is easy to add new  commands into an existing modules. The command class is just a simple structure holding 
the description of the command and nothing else. A Command can contain five fields: 

  * `const char* name`: The name of this command, this is the name wich will be used to refer to this command in
    the module
  * `const char* help`: The help of this command. This help will be print when the user use the builtin command `help`
  * `const ConstArray<CommandArgDescription> argsDescription`: Description of each args of the command. This is used 
    by the builtin command `args` and by the runtime wich check if arguments passed to this commands contain at least
    the same number of arguments that this array contains.
  * `uint8_t maximumArgsRequired`: The maximum args count required. If specified, this will be the upper bound for 
    args checks. 
  * `const CommandHandler_t handler`: The command handler associated  with this command, it take Args in input and 
    return `CommandResult` in outpu

It is not required to fullfil all fields of the command to make it work. THe best is to look at the class reference.


#### Full example:

```C++
static constexpr const Command setAddress { 
  /* help */ "setAddress",
  /* names */ "set the address of this device",
  /* argsDescription */ (const CommandArgDescription[]) {
    { "<addressType>", "The address type to set. It is a string representation of Gap::AddressType_t" },
    { "<address>"    , "The address itself which is a string representation like \"XX:XX:XX:XX:XX:XX\"" },    
  },
  /* handler */
  STATIC_LAMBDA(const CommandArgs& args) {
    // extract first args 
    Gap::AddressType_t addressType;
    if(!fromString(args[0], addressType)) {
      return CommandResult::invalidParameters("first argument should match Gap::AddressType_t");
    }

    Gap::Address_t address;
    if(!macAddressFromString(args[1], address)) { 
      return CommandResult::invalidParameters("second argument should is a mac address which should match XX:XX:XX:XX:XX:XX format");
    }

    ble_error_t err = gap().setAddress(addressType, address);

    return err ? CommandResult::faillure(to_string(err)) : CommandResult::success();
  }
};
```


#### Tips and note

* At the moment, it is recommended to use compound litterals for argsDescription initialization
* Make the command constexpr, it will save RAM. ** static const is not enough !!!***
* If you want to initialize the command handler inline. The C++ language does not allow lambda to be constexpr 
  but you can use the STATIC_LAMBDA workaround.
* At some point, commands should be referenced by modules, don't forget to add your new command into its module.



### Modules

Modules regroup command togethers into a CommandSuite. A CommandSuite needs a CommandSuiteDescription to be run. To 
provide a CommandSuiteDescription, you just have to provide a class or a structure whivh contains the correct fields : 

#### Description
  * static const char* name() : The name of the suite. Each commands present in the suite
    will be available through this entry point.
  * static const char* info() : Informations about this command suite 
  * static const char* man() : The manual of this command suite 
  * static ConstArray<Command> commands() : The array of commands presents in the suite

#### Instantiation

In the main.cpp, register your CommandSuite in the function `void initialize_app_commands(void)` by calling the function template : 

```
registerCommandSuite<CommandSuiteDescriptionType>();
```

#### Full example 

```C++
class BLECommandSuiteDescription {

public:
  static const char* name() { 
    return "ble";
  } 
 
  static const char* info() {
    return "All commands applicable to BLE instance";
  } 

  static const char* man() {
    return "BLE <command> <command arguments>\r\n"\
           "  * ble shutdown : shutdown the ble instance of this device\r\n"\
           "  * ble init : initialise the ble instance of this device\r\n"\
           "  * ble reset : reset the ble instance of this device\r\n"\
           "  * ble get_version : return the current version of the BLE stack\r\n"; 
  } 

  // see implementation
  static ConstArray<Command> commands();
};
```












