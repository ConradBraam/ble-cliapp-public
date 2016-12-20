#ifndef BLE_CLIAPP_CLICOMMAND_COMMAND_HANDLER_GENERATOR_H_
#define BLE_CLIAPP_CLICOMMAND_COMMAND_HANDLER_GENERATOR_H_

#include "Command.h"
#include "Serializer.h"
#include "Hex.h"


struct CommandHandlerGenerator { 

    template<typename CommandType>
    static void handler(const CommandArgs& args, const CommandResponsePtr& response) { 
        CommandHandlerGenerator::generated_handler(args, response, CommandType::handler, CommandType::argsDescription);
    }

private:
    template<typename T>
    struct remove_reference { 
        typedef T type;
    };


    template<typename T>
    struct remove_reference<T&> { 
        typedef T type;
    };


    static void print_error(
        const CommandResponsePtr& response, 
        uint32_t index, 
        ConstArray<CommandArgDescription> (*argsDescription)()
    );


    static void generated_handler(const CommandArgs& args, const CommandResponsePtr& response, 
                        void(*_real_handler)(const CommandArgs&, const CommandResponsePtr&),
                        ConstArray<CommandArgDescription> (*argsDescription)()) { 
        _real_handler(args, response);
    }


    static void generated_handler(const CommandArgs& args, const CommandResponsePtr& response, 
                        void(*_real_handler)(const CommandResponsePtr&),
                        ConstArray<CommandArgDescription> (*argsDescription)()) { 
        _real_handler(response);
    }


    template<typename A0>
    static void generated_handler(const CommandArgs& args, const CommandResponsePtr& response, 
                        void(*_real_handler)(A0, const CommandResponsePtr&),
                        ConstArray<CommandArgDescription> (*argsDescription)()) { 
        typename remove_reference<A0>::type arg0;
        if (!fromString(args[0], arg0)) {
            print_error(response, 0, argsDescription);
            return;
        }

        _real_handler(arg0, response);
    }


    template<typename A0, typename A1>
    static void generated_handler(const CommandArgs& args, const CommandResponsePtr& response, 
                        void(*_real_handler)(A0, A1, const CommandResponsePtr&),
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
    static void generated_handler(const CommandArgs& args, const CommandResponsePtr& response, 
                        void(*_real_handler)(A0, A1, A2, const CommandResponsePtr&),
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
    static void generated_handler(const CommandArgs& args, const CommandResponsePtr& response, 
                        void(*_real_handler)(A0, A1, A2, A3, const CommandResponsePtr&),
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
    static void generated_handler(const CommandArgs& args, const CommandResponsePtr& response, 
                        void(*_real_handler)(A0, A1, A2, A3, A4, const CommandResponsePtr&),
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
    static void generated_handler(const CommandArgs& args, const CommandResponsePtr& response, 
                        void(*_real_handler)(A0, A1, A2, A3, A4, A5, const CommandResponsePtr&),
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
    static void generated_handler(const CommandArgs& args, const CommandResponsePtr& response, 
                        void(*_real_handler)(A0, A1, A2, A3, A4, A5, A6, const CommandResponsePtr&),
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
    static void generated_handler(const CommandArgs& args, const CommandResponsePtr& response, 
                        void(*_real_handler)(A0, A1, A2, A3, A4, A5, A6, A7, const CommandResponsePtr&),
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
    static void generated_handler(const CommandArgs& args, const CommandResponsePtr& response, 
                        void(*_real_handler)(A0, A1, A2, A3, A4, A5, A6, A7, A8, const CommandResponsePtr&),
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
    static void generated_handler(const CommandArgs& args, const CommandResponsePtr& response, 
                        void(*_real_handler)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, const CommandResponsePtr&),
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
    static void generated_handler(const CommandArgs& args, const CommandResponsePtr& response, 
                        void(*_real_handler)(A0, A1, A2, A3, A4, A5, A6, A7, A8, A9, A10, const CommandResponsePtr&),
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


#endif //BLE_CLIAPP_CLICOMMAND_COMMAND_HANDLER_GENERATOR_H_
