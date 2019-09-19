//
// Created by serik1987 on 20.09.19.
//

#ifndef MPI2_LOGGER_H
#define MPI2_LOGGER_H

#include <string>
#include <syslog.h>

namespace logging {

    class Logger {

    public:
        Logger() {};

        Logger(const Logger &other) = delete;

        Logger &operator=(const Logger &other) = delete;

        enum Priority {
            Debug = LOG_DEBUG, Info = LOG_INFO, Notice = LOG_NOTICE, Warning = LOG_WARNING,
            Error = LOG_ERR, Failed = LOG_ALERT
        };

        virtual void writeLog(const std::string &msg, Priority priority, bool print_priority,
                bool print_timestamp) = 0;

    protected:
        std::string makeString(const std::string &msg, Priority priority, bool print_priority,
                               bool print_timestamp);
    };

}

#endif //MPI2_LOGGER_H
