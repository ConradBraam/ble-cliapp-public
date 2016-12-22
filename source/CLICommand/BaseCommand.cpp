#include "BaseCommand.h"
#include "mbed_error.h"


const char* BaseCommand::help() {
    return "";
}


ConstArray<CommandArgDescription> BaseCommand::argsDescription() {
    return ConstArray<CommandArgDescription>();
}


ConstArray<CommandArgDescription> BaseCommand::resultDescription() {
    return ConstArray<CommandArgDescription>();
}
