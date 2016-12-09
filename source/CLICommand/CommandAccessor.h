#ifndef BLE_CLIAPP_CLICOMMAND_COMMAND_ACCESSOR_H_
#define BLE_CLIAPP_CLICOMMAND_COMMAND_ACCESSOR_H_

#include "Command.h"
#include "Serializer.h"
#include "Hex.h"

template<typename T>
struct CommandAccessor { 
    static const Command command;
};

template<typename T>
struct remove_reference { 
    typedef T type;
};

template<typename T>
struct remove_reference<T&> { 
    typedef T type;
};


struct CommandHandlerSynthetizer { 

    static void print_error(const mbed::util::SharedPointer<CommandResponse>& response, 
        uint32_t index, ConstArray<CommandArgDescription> (*argsDescription)()) { 
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


    static void handler(const CommandArgs& args, const mbed::util::SharedPointer<CommandResponse>& response, 
                        void(*_real_handler)(const CommandArgs&, const mbed::util::SharedPointer<CommandResponse>&),
                        ConstArray<CommandArgDescription> (*argsDescription)()) { 
        _real_handler(args, response);
    }


    static void handler(const CommandArgs& args, const mbed::util::SharedPointer<CommandResponse>& response, 
                        void(*_real_handler)(const mbed::util::SharedPointer<CommandResponse>&),
                        ConstArray<CommandArgDescription> (*argsDescription)()) { 
        _real_handler(response);
    }

    template<typename A0>
    static void handler(const CommandArgs& args, const mbed::util::SharedPointer<CommandResponse>& response, 
                        void(*_real_handler)(A0, const mbed::util::SharedPointer<CommandResponse>&),
                        ConstArray<CommandArgDescription> (*argsDescription)()) { 
        typename remove_reference<A0>::type arg0;
        if (!fromString(args[0], arg0)) {
            print_error(response, 0, argsDescription);
            return;
        }

        _real_handler(arg0, response);
    }


    template<typename A0, typename A1>
    static void handler(const CommandArgs& args, const mbed::util::SharedPointer<CommandResponse>& response, 
                        void(*_real_handler)(A0, A1, const mbed::util::SharedPointer<CommandResponse>&),
                        ConstArray<CommandArgDescription> (*argsDescription)()) { 
        typename remove_reference<A0>::type arg0;
        if (!fromString(args[0], arg0)) {
            print_error(response, 0, argsDescription);
            return;
        }

        typename remove_reference<A1>::type arg1;
        if (!fromString(args[1], arg1)) {
            print_error(response, 1, argsDescription);
            return;
        }        

        _real_handler(arg0, arg1, response);
    }

    template<typename A0, typename A1, typename A2>
    static void handler(const CommandArgs& args, const mbed::util::SharedPointer<CommandResponse>& response, 
                        void(*_real_handler)(A0, A1, A2, const mbed::util::SharedPointer<CommandResponse>&),
                        ConstArray<CommandArgDescription> (*argsDescription)()) { 
        typename remove_reference<A0>::type arg0;
        if (!fromString(args[0], arg0)) {
            print_error(response, 0, argsDescription);
            return;
        }

        typename remove_reference<A1>::type arg1;
        if (!fromString(args[1], arg1)) {
            print_error(response, 1, argsDescription);
            return;
        }        

        typename remove_reference<A2>::type arg2;
        if (!fromString(args[2], arg2)) {
            print_error(response, 2, argsDescription);
            return;
        }        
        _real_handler(arg0, arg1, arg2, response);
    }


    template<typename A0, typename A1, typename A2, typename A3>
    static void handler(const CommandArgs& args, const mbed::util::SharedPointer<CommandResponse>& response, 
                        void(*_real_handler)(A0, A1, A2, A3, const mbed::util::SharedPointer<CommandResponse>&),
                        ConstArray<CommandArgDescription> (*argsDescription)()) { 
        typename remove_reference<A0>::type arg0;
        if (!fromString(args[0], arg0)) {
            print_error(response, 0, argsDescription);
            return;
        }

        typename remove_reference<A1>::type arg1;
        if (!fromString(args[1], arg1)) {
            print_error(response, 1, argsDescription);
            return;
        }        

        typename remove_reference<A2>::type arg2;
        if (!fromString(args[2], arg2)) {
            print_error(response, 2, argsDescription);
            return;
        }        

        typename remove_reference<A3>::type arg3;
        if (!fromString(args[3], arg3)) {
            print_error(response, 3, argsDescription);
            return;
        }        

        _real_handler(arg0, arg1, arg2, arg3, response);
    }

    template<typename A0, typename A1, typename A2, typename A3, typename A4>
    static void handler(const CommandArgs& args, const mbed::util::SharedPointer<CommandResponse>& response, 
                        void(*_real_handler)(A0, A1, A2, A3, A4, const mbed::util::SharedPointer<CommandResponse>&),
                        ConstArray<CommandArgDescription> (*argsDescription)()) { 
        typename remove_reference<A0>::type arg0;
        if (!fromString(args[0], arg0)) {
            print_error(response, 0, argsDescription);
            return;
        }

        typename remove_reference<A1>::type arg1;
        if (!fromString(args[1], arg1)) {
            print_error(response, 1, argsDescription);
            return;
        }        

        typename remove_reference<A2>::type arg2;
        if (!fromString(args[2], arg2)) {
            print_error(response, 2, argsDescription);
            return;
        }        

        typename remove_reference<A3>::type arg3;
        if (!fromString(args[3], arg3)) {
            print_error(response, 3, argsDescription);
            return;
        }        

        typename remove_reference<A4>::type arg4;
        if (!fromString(args[4], arg4)) {
            print_error(response, 4, argsDescription);
            return;
        }     

        _real_handler(arg0, arg1, arg2, arg3, arg4, response);
    }

    template<typename A0, typename A1, typename A2, typename A3, typename A4, typename A5>
    static void handler(const CommandArgs& args, const mbed::util::SharedPointer<CommandResponse>& response, 
                        void(*_real_handler)(A0, A1, A2, A3, A4, A5, const mbed::util::SharedPointer<CommandResponse>&),
                        ConstArray<CommandArgDescription> (*argsDescription)()) { 
        typename remove_reference<A0>::type arg0;
        if (!fromString(args[0], arg0)) {
            print_error(response, 0, argsDescription);
            return;
        }

        typename remove_reference<A1>::type arg1;
        if (!fromString(args[1], arg1)) {
            print_error(response, 1, argsDescription);
            return;
        }        

        typename remove_reference<A2>::type arg2;
        if (!fromString(args[2], arg2)) {
            print_error(response, 2, argsDescription);
            return;
        }        

        typename remove_reference<A3>::type arg3;
        if (!fromString(args[3], arg3)) {
            print_error(response, 3, argsDescription);
            return;
        }        

        typename remove_reference<A4>::type arg4;
        if (!fromString(args[4], arg4)) {
            print_error(response, 4, argsDescription);
            return;
        }     

        typename remove_reference<A5>::type arg5;
        if (!fromString(args[5], arg5)) {
            print_error(response, 5, argsDescription);
            return;
        }     

        _real_handler(arg0, arg1, arg2, arg3, arg4, arg5, response);
    }



    template<typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, 
             typename A6>
    static void handler(const CommandArgs& args, const mbed::util::SharedPointer<CommandResponse>& response, 
                        void(*_real_handler)(A0, A1, A2, A3, A4, A5, A6, const mbed::util::SharedPointer<CommandResponse>&),
                        ConstArray<CommandArgDescription> (*argsDescription)()) { 
        typename remove_reference<A0>::type arg0;
        if (!fromString(args[0], arg0)) {
            print_error(response, 0, argsDescription);
            return;
        }

        typename remove_reference<A1>::type arg1;
        if (!fromString(args[1], arg1)) {
            print_error(response, 1, argsDescription);
            return;
        }        

        typename remove_reference<A2>::type arg2;
        if (!fromString(args[2], arg2)) {
            print_error(response, 2, argsDescription);
            return;
        }        

        typename remove_reference<A3>::type arg3;
        if (!fromString(args[3], arg3)) {
            print_error(response, 3, argsDescription);
            return;
        }        

        typename remove_reference<A4>::type arg4;
        if (!fromString(args[4], arg4)) {
            print_error(response, 4, argsDescription);
            return;
        }     

        typename remove_reference<A5>::type arg5;
        if (!fromString(args[5], arg5)) {
            print_error(response, 5, argsDescription);
            return;
        }     

        typename remove_reference<A6>::type arg6;
        if (!fromString(args[6], arg6)) {
            print_error(response, 6, argsDescription);
            return;
        }

        _real_handler(arg0, arg1, arg2, arg3, arg4, arg5, arg6, response);
    }


    template<typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, 
             typename A6, typename A7>
    static void handler(const CommandArgs& args, const mbed::util::SharedPointer<CommandResponse>& response, 
                        void(*_real_handler)(A0, A1, A2, A3, A4, A5, A6, A7, const mbed::util::SharedPointer<CommandResponse>&),
                        ConstArray<CommandArgDescription> (*argsDescription)()) { 
        typename remove_reference<A0>::type arg0;
        if (!fromString(args[0], arg0)) {
            print_error(response, 0, argsDescription);
            return;
        }

        typename remove_reference<A1>::type arg1;
        if (!fromString(args[1], arg1)) {
            print_error(response, 1, argsDescription);
            return;
        }        

        typename remove_reference<A2>::type arg2;
        if (!fromString(args[2], arg2)) {
            print_error(response, 2, argsDescription);
            return;
        }        

        typename remove_reference<A3>::type arg3;
        if (!fromString(args[3], arg3)) {
            print_error(response, 3, argsDescription);
            return;
        }        

        typename remove_reference<A4>::type arg4;
        if (!fromString(args[4], arg4)) {
            print_error(response, 4, argsDescription);
            return;
        }     

        typename remove_reference<A5>::type arg5;
        if (!fromString(args[5], arg5)) {
            print_error(response, 5, argsDescription);
            return;
        }     

        typename remove_reference<A6>::type arg6;
        if (!fromString(args[6], arg6)) {
            print_error(response, 6, argsDescription);
            return;
        }

        typename remove_reference<A7>::type arg7;
        if (!fromString(args[7], arg7)) {
            print_error(response, 7, argsDescription);
            return;
        }     

        _real_handler(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, response);
    }

    template<typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, 
             typename A6, typename A7, typename A8>
    static void handler(const CommandArgs& args, const mbed::util::SharedPointer<CommandResponse>& response, 
                        void(*_real_handler)(A0, A1, A2, A3, A4, A5, A6, A7, A8, const mbed::util::SharedPointer<CommandResponse>&),
                        ConstArray<CommandArgDescription> (*argsDescription)()) { 
        typename remove_reference<A0>::type arg0;
        if (!fromString(args[0], arg0)) {
            print_error(response, 0, argsDescription);
            return;
        }

        typename remove_reference<A1>::type arg1;
        if (!fromString(args[1], arg1)) {
            print_error(response, 1, argsDescription);
            return;
        }        

        typename remove_reference<A2>::type arg2;
        if (!fromString(args[2], arg2)) {
            print_error(response, 2, argsDescription);
            return;
        }        

        typename remove_reference<A3>::type arg3;
        if (!fromString(args[3], arg3)) {
            print_error(response, 3, argsDescription);
            return;
        }        

        typename remove_reference<A4>::type arg4;
        if (!fromString(args[4], arg4)) {
            print_error(response, 4, argsDescription);
            return;
        }     

        typename remove_reference<A5>::type arg5;
        if (!fromString(args[5], arg5)) {
            print_error(response, 5, argsDescription);
            return;
        }     

        typename remove_reference<A6>::type arg6;
        if (!fromString(args[6], arg6)) {
            print_error(response, 6, argsDescription);
            return;
        }

        typename remove_reference<A7>::type arg7;
        if (!fromString(args[7], arg7)) {
            print_error(response, 7, argsDescription);
            return;
        }     

        typename remove_reference<A8>::type arg8;
        if (!fromString(args[8], arg8)) {
            print_error(response, 8, argsDescription);
            return;
        }     

        _real_handler(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, response);
    }

    template<typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, 
             typename A6, typename A7, typename A8, typename A9>
    static void handler(const CommandArgs& args, const mbed::util::SharedPointer<CommandResponse>& response, 
                        void(*_real_handler)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, const mbed::util::SharedPointer<CommandResponse>&),
                        ConstArray<CommandArgDescription> (*argsDescription)()) { 
        typename remove_reference<A0>::type arg0;
        if (!fromString(args[0], arg0)) {
            print_error(response, 0, argsDescription);
            return;
        }

        typename remove_reference<A1>::type arg1;
        if (!fromString(args[1], arg1)) {
            print_error(response, 1, argsDescription);
            return;
        }        

        typename remove_reference<A2>::type arg2;
        if (!fromString(args[2], arg2)) {
            print_error(response, 2, argsDescription);
            return;
        }        

        typename remove_reference<A3>::type arg3;
        if (!fromString(args[3], arg3)) {
            print_error(response, 3, argsDescription);
            return;
        }        

        typename remove_reference<A4>::type arg4;
        if (!fromString(args[4], arg4)) {
            print_error(response, 4, argsDescription);
            return;
        }     

        typename remove_reference<A5>::type arg5;
        if (!fromString(args[5], arg5)) {
            print_error(response, 5, argsDescription);
            return;
        }     

        typename remove_reference<A6>::type arg6;
        if (!fromString(args[6], arg6)) {
            print_error(response, 6, argsDescription);
            return;
        }

        typename remove_reference<A7>::type arg7;
        if (!fromString(args[7], arg7)) {
            print_error(response, 7, argsDescription);
            return;
        }     

        typename remove_reference<A8>::type arg8;
        if (!fromString(args[8], arg8)) {
            print_error(response, 8, argsDescription);
            return;
        }     

        typename remove_reference<A9>::type arg9;
        if (!fromString(args[9], arg9)) {
            print_error(response, 9, argsDescription);
            return;
        }

        _real_handler(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, response);
    }

    template<typename A0, typename A1, typename A2, typename A3, typename A4, typename A5, 
             typename A6, typename A7, typename A8, typename A9, typename A10>
    static void handler(const CommandArgs& args, const mbed::util::SharedPointer<CommandResponse>& response, 
                        void(*_real_handler)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, const mbed::util::SharedPointer<CommandResponse>&),
                        ConstArray<CommandArgDescription> (*argsDescription)()) { 
        typename remove_reference<A0>::type arg0;
        if (!fromString(args[0], arg0)) {
            print_error(response, 0, argsDescription);
            return;
        }

        typename remove_reference<A1>::type arg1;
        if (!fromString(args[1], arg1)) {
            print_error(response, 1, argsDescription);
            return;
        }        

        typename remove_reference<A2>::type arg2;
        if (!fromString(args[2], arg2)) {
            print_error(response, 2, argsDescription);
            return;
        }        

        typename remove_reference<A3>::type arg3;
        if (!fromString(args[3], arg3)) {
            print_error(response, 3, argsDescription);
            return;
        }        

        typename remove_reference<A4>::type arg4;
        if (!fromString(args[4], arg4)) {
            print_error(response, 4, argsDescription);
            return;
        }     

        typename remove_reference<A5>::type arg5;
        if (!fromString(args[5], arg5)) {
            print_error(response, 5, argsDescription);
            return;
        }     

        typename remove_reference<A6>::type arg6;
        if (!fromString(args[6], arg6)) {
            print_error(response, 6, argsDescription);
            return;
        }

        typename remove_reference<A7>::type arg7;
        if (!fromString(args[7], arg7)) {
            print_error(response, 7, argsDescription);
            return;
        }     

        typename remove_reference<A8>::type arg8;
        if (!fromString(args[8], arg8)) {
            print_error(response, 8, argsDescription);
            return;
        }     

        typename remove_reference<A9>::type arg9;
        if (!fromString(args[9], arg9)) {
            print_error(response, 9, argsDescription);
            return;
        }             

        typename remove_reference<A10>::type arg10;
        if (!fromString(args[10], arg10)) {
            print_error(response, 10, argsDescription);
            return;
        }     

        _real_handler(arg0, arg1, arg2, arg3, arg4, arg5, arg6, arg7, arg8, arg9, arg10, response);
    }
};


template<typename Command>
struct CommandHandlerResolver { 
    static void handler(const CommandArgs& args, const mbed::util::SharedPointer<CommandResponse>& response) { 
        CommandHandlerSynthetizer::handler(args, response, Command::handler, Command::argsDescription);
    }
};


template<typename T>
const Command CommandAccessor<T>::command =  {
        &T::name,
#if defined(ENABLE_COMMAND_HELP)
        &T::help,
#endif
        &T::argsDescription,
        &T::template maximumArgsRequired<T>,
        &CommandHandlerResolver<T>::handler
};

typedef const Command* (*CommandAccessor_t)();

#endif //BLE_CLIAPP_CLICOMMAND_COMMAND_ACCESSOR_H_
