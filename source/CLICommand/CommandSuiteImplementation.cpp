#include <stdint.h>
#include <minar/minar.h>

#include "util/StaticLambda.h"
#include "CommandSuiteImplementation.h"

namespace {

static void whenAsyncCommandEnd(const CommandResponse* response) {
    minar::Scheduler::postCallback(mbed::util::FunctionPointer1<void, int>(cmd_ready).bind(response->getStatusCode()));
}

static const Command* getCommand(const char* name, const ConstArray<Command>& builtinCommands, const ConstArray<Command>& moduleCommands) {
    // builtin commands
    for(size_t i = 0; i < builtinCommands.count(); ++i) {
        if(strcmp(name, builtinCommands[i].name) == 0) {
            return &builtinCommands[i];
        }
    }

    for(size_t i = 0; i < moduleCommands.count(); ++i) {
        if(strcmp(name, moduleCommands[i].name) == 0) {
            return &moduleCommands[i];
        }
    }

    return nullptr;
}

}

int CommandSuiteImplementation::commandHandler(int argc, char** argv, const ConstArray<Command>& builtinCommands, const ConstArray<Command>& moduleCommands) {
    const CommandArgs args(argc, argv);
    const char* commandName = args[1];
    const CommandArgs commandArgs(args.drop(2));

    std::shared_ptr<CommandResponse> response = std::make_shared<CommandResponse>();

    response->setCommandName(commandName);
    response->setArguments(commandArgs);

    const Command* command = getCommand(commandName, builtinCommands, moduleCommands);
    if(!command) {
        response->faillure("invalid command name, you can get all the command name for this module by using the command 'list'");
        return response->getStatusCode();
    }

    // check arguments
    if(commandArgs.count() < command->argsDescription.count()) {
        response->invalidParameters("not enough arguments");
        return response->getStatusCode();
    }

    if(commandArgs.count() > command->maximumArgsRequired) {
        response->invalidParameters("too many arguments");
        return response->getStatusCode();
    }

    // execute the handler
    command->handler(commandArgs, response);

    // if response is not referenced elsewhere, this means that the execution is done,
    // just return the status code set
    // otherwise, tell the system that the execution continue and install continuation
    // callback
    if(response.unique()) {
        return response->getStatusCode();
    } else {
        response->setOnClose(whenAsyncCommandEnd);
        return CMDLINE_RETCODE_EXCUTING_CONTINUE;
    }
}

void CommandSuiteImplementation::help(const CommandArgs& args, const std::shared_ptr<CommandResponse>& response,
    const ConstArray<Command>& builtinCommands, const ConstArray<Command>& moduleCommands) {
    const Command* command = getCommand(args[0], builtinCommands, moduleCommands);
    if(!command) {
        response->invalidParameters("the name of this command does not exist, you can list the command by using the command 'list'");
    } else {
        response->success(command->help);
    }
}

void CommandSuiteImplementation::list(const CommandArgs&, const std::shared_ptr<CommandResponse>& response,
    const ConstArray<Command>& builtinCommands, const ConstArray<Command>& moduleCommands) {
    using namespace serialization;

    response->setStatusCode(CommandResponse::SUCCESS);

    auto& os = response->getResultStream();

    os << startArray;
    // builtin commands
    for(size_t i = 0; i < builtinCommands.count(); ++i) {
        os << builtinCommands[i].name;
    }

    for(size_t i = 0; i < moduleCommands.count(); ++i) {
        os << moduleCommands[i].name;
    }

    os << endArray;
}

void CommandSuiteImplementation::args(const CommandArgs& args, const std::shared_ptr<CommandResponse>& response,
    const ConstArray<Command>& builtinCommands, const ConstArray<Command>& moduleCommands) {
    using namespace serialization;

    const Command* command = getCommand(args[0], builtinCommands, moduleCommands);
    if(!command) {
        response->invalidParameters("the name of this command does not exist, you can list the command by using the command 'list'");
        return;
    }

    auto& os = response->getResultStream();

    os << startArray;
    for(size_t i = 0; i < command->argsDescription.count(); ++i) {
        os << startObject << key(command->argsDescription[i].name) << command->argsDescription[i].desc << endObject;
    }
    os << endArray;
}
