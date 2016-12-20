#include "CommandHandlerGenerator.h"
#include "Serializer.h"

void CommandHandlerGenerator::print_error(
    const CommandResponsePtr& response, 
    uint32_t index, 
    ConstArray<CommandArgDescription> (*argsDescription)()) { 
    using namespace serialization;

    response->setStatusCode(CommandResponse::INVALID_PARAMETERS);

    JSONOutputStream& out = response->getResultStream();

    out << startObject <<
        key("index") << index;

    ConstArray<CommandArgDescription> args_desc = argsDescription();
    if (args_desc.count() > index) { 
        out <<
        key("name") << args_desc[index].name <<
        key("type") << args_desc[index].type <<
        key("description") << args_desc[index].desc;
    } 
    out << endObject;
}
