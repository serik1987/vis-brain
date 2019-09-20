//
// Created by serik1987 on 20.09.19.
//

#include "SystemLogger.h"
#include "../compile_options.h"
#include "exceptions.h"

namespace logging {

    bool SystemLogger::logger_exists = false;

    SystemLogger::SystemLogger() {
        if (logger_exists){
            throw system_logger_already_exists();
        }
        openlog(APP_NAME, LOG_NDELAY | LOG_PID, LOG_USER);
        logger_exists = true;
    }

    SystemLogger::~SystemLogger(){
        closelog();
        logger_exists = false;
    }

    void SystemLogger::writeLog(const std::string &msg, logging::Logger::Priority priority, bool print_priority,
                                bool print_timestamp) {
        std::string log_message = makeString(msg, priority, print_priority, print_timestamp);
        syslog(LOG_USER | (int)priority, log_message.c_str());
    }

}
