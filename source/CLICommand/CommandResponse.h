#ifndef BLE_CLIAPP_COMMAND_RESPONSE_H_
#define BLE_CLIAPP_COMMAND_RESPONSE_H_

#include <mbed-client-cli/ns_cmdline.h>
#include <mbed-client-cli/ns_types.h>

#include "CommandArgs.h"
#include "Serialization/JSONOutputStream.h"

#include <core-util/FunctionPointer.h>

class CommandResponse {

public:
    typedef mbed::util::FunctionPointer1<void, const CommandResponse*> OnClose_t;

    /**
     * @brief construct a new response
     * @param outputStream The output stream used to write the response.
     * @important It is expected that outputStream reference remain valid until
     * the response has been closed.
     */
    CommandResponse(const OnClose_t& onCloseCallBack);

    /**
     * @brief Destroy the command response and close the stream if the stream was not
     * already closed.
     */
    ~CommandResponse();

    enum StatusCode_t {
        COMMAND_BUSY =             2,   //!< Command Busy
        EXCUTING_CONTINUE =        1,  //!< Execution continue in background
        SUCCESS =                  0,   //!< Execution Success
        FAIL =                    -1,   //!< Execution Fail
        INVALID_PARAMETERS =      -2,   //!< Command parameters was incorrect
        COMMAND_NOT_IMPLEMENTED = -3,   //!< Command not implemented
        COMMAND_CB_MISSING =      -4,   //!< Command callback function missing
        COMMAND_NOT_FOUND =       -5   //!< Command not found
    };

    /**
     * @brief Set the command name associated with this response. If the
     * command name has been already set, this function will return false.
     * @param name The name of the command associated with this response.
     * @return true if the command name has been set and false otherwise
     */
    bool setCommandName(const char* name);

    /**
     * @brief Set the arguments associated with this response. If the arguments
     * have been already set, this function will return false.
     * @param args Arguments associated with this response
     * @return true if the arguments has been correctly set and false otherwise.
     */
    bool setArguments(const CommandArgs& args);

    /**
     * @brief Set the status code of the response, if it has already been set, the
     * function will return false.
     * @param statusCode The status code of the response
     * @return true if the status code has been correctly set and false otherwise
     */
    bool setStatusCode(StatusCode_t statusCode);

    /**
     * @brief return the current status code
     */
    StatusCode_t getStatusCode() const;

    /**
     * @brief Get the JSON stream of the response. User are expected to use this
     * stream to write the body of the response.
     * @note it is expected that setStatusCode, setCommandName and setArguments has
     * been called prior to this call.
     * @note It is expected that the user format properly the body of the response.
     * @important Use of the result of this function while isClosed() == true is undefined.
     * @important It is expected that the user does not call setStatusCode, setArgument
     * and setNameFunction until the result has been properly formed.
     * @return a JSON output stream.
     */
    serialization::JSONOutputStream& getResultStream();

    /**
     * @brief Ask to close the response
     */
    void close();

    /**
     * @brief Indicate if the response has been closed or not.
     * @return true if the response has been closed and false otherwise
     */
    bool isClosed();

private:
    OnClose_t onClose;
    serialization::JSONOutputStream out;
    StatusCode_t statusCode;
    bool nameSet:1;
    bool argumentsSet:1;
    bool statusCodeSet:1;
    bool resultStarted:1;
    bool closed:1;
};



#endif //BLE_CLIAPP_COMMAND_RESPONSE_H_
