//
// Created by serik1987 on 20.09.19.
//

#ifndef MPI2_LOGGER_H
#define MPI2_LOGGER_H

#include <string>
#include <syslog.h>

namespace logging {

    /**
     * Base class for any logger
     */
    class Logger {

    public:
        Logger() {};

        Logger(const Logger &other) = delete;

        Logger &operator=(const Logger &other) = delete;

        /**
         * Represents the logger priority
         *
         * Debug - debug messages [white alarm level]: they will be printed on screen, in debug.log and in output/debug.log when
         * DEBUG macro is 1 (see compiling_options.h). The messages will be ignored is DEBUF macro is 0
         *
         * Info - info messages[black alarm level]: contain model parameters for control. To be printed to
         * output/model-parameters.log.
         *
         * Notice - information for the progress bar [green alarm level]. The messages represent in the format
         * x:y:info where x - number of stages that has already been completed, y - number of stages to be completed
         * info - information about certain stage
         *
         * Warning - warning messages [yellow alarm level]. The parameters you entered can't be applied, so
         * some of them were automatically changed. Detailed information is given in the message.
         * The message will be printed in output/model-parameters.log and output.warning.log
         *
         * Error - critical error [magenta alarm level]. The simulation was aborted due to internal error. The program
         * will start the next simulation. All simulation results will be deleted or set to nan.
         *
         * Failed - highly critical error [red alarm level]. The program can not longer perform the job and will be
         * terminated.
         */
        enum Priority {
            Debug = LOG_DEBUG, Info = LOG_INFO, Notice = LOG_NOTICE, Warning = LOG_WARNING,
            Error = LOG_ERR, Failed = LOG_ALERT
        };

        /**
         * Writes the log to the logger device
         *
         * @param msg the message itself
         * @param priority priority (see help on logging::Logger::Priority)
         * @param print_priority true if you want to print priority level in the logging message
         * @param print_timestamp true if you want to print timestamp and rank of the process in the logging message
         */
        virtual void writeLog(const std::string &msg, Priority priority, bool print_priority,
                bool print_timestamp) = 0;

    protected:
        std::string makeString(const std::string &msg, Priority priority, bool print_priority,
                               bool print_timestamp);
    };

}

#endif //MPI2_LOGGER_H
