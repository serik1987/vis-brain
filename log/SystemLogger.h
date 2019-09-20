//
// Created by serik1987 on 20.09.19.
//

#ifndef MPI2_SYSTEMLOGGER_H
#define MPI2_SYSTEMLOGGER_H

#include "Logger.h"

namespace logging {


    /**
     * Prints logs using syslogd facilities
     */
    class SystemLogger: public Logger {
    private:
        static bool logger_exists;
    public:
        SystemLogger();
        ~SystemLogger();
        virtual void writeLog(const std::string &msg, Priority priority, bool print_priority,
                              bool print_timestamp);
    };

}


#endif //MPI2_SYSTEMLOGGER_H
