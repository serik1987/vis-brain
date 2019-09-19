//
// Created by serik1987 on 20.09.19.
//

#include <sstream>
#include <iomanip>
#include "Logger.h"
#include "../mpi/Communicator.h"
#include "../Application.h"

namespace logging{

    std::string Logger::makeString(const std::string &msg, logging::Logger::Priority priority, bool print_priority,
                            bool print_timestamp) {
        std::ostringstream buffer;
        if (print_timestamp){
            Application& app = Application::getInstance();
            mpi::Communicator& comm = app.getAppCommunicator();
            buffer << std::fixed << "[ " << comm.getRank() << "/" << comm.getProcessorNumber() <<
                " | " << std::setprecision(6) << app.getLocalDifference() << " s | " <<
                app.getGlobalDifference() << " s since start ] ";
        }
        if (print_priority){
            switch (priority){
                case Warning:
                    buffer << "WARNING: ";
                    break;
                case Error:
                    buffer << "ERROR: ";
                    break;
                case Failed:
                    buffer << "FATAL ERROR: ";
                    break;
            }
        }
        buffer << msg << "\n";
        return buffer.str();
    }

}