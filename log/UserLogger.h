//
// Created by serik1987 on 20.09.19.
//

#ifndef MPI2_USERLOGGER_H
#define MPI2_USERLOGGER_H

#include "Logger.h"
#include "../mpi/File.h"

namespace logging {

    /**
     * Prints logs to the user file using MPI facilities
     */
    class UserLogger: public Logger {
    private:
        mpi::File* file;
    public:
        UserLogger(const std::string& filename);
        ~UserLogger();
        void writeLog(const std::string &msg, Priority priority, bool print_priority,
                      bool print_timestamp) override;
    };

}


#endif //MPI2_USERLOGGER_H
