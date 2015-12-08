# add -funroll loops to the compile commands used for the sources in this
# module... loops deserve some fun too!
set_target_properties("ble-cliapp" PROPERTIES COMPILE_FLAGS "-std=c++11")