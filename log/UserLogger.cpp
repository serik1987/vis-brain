//
// Created by serik1987 on 20.09.19.
//

#include "UserLogger.h"
#include "../Application.h"

namespace logging{

    UserLogger::UserLogger(const std::string &filename): Logger() {
        mpi::Communicator& comm = Application::getInstance().getAppCommunicator();
        file = new mpi::File(comm, filename, MPI_MODE_WRONLY | MPI_MODE_CREATE);
    }

    UserLogger::~UserLogger() {
        delete file;
    }

    void
    UserLogger::writeLog(const std::string &msg, Logger::Priority priority, bool print_priority, bool print_timestamp) {
        std::string final_message = makeString(msg, priority, print_priority, print_timestamp);
        file->writeShared(final_message.c_str(), final_message.length(), MPI_CHAR);
    }

}
