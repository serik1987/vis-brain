//
// Created by serik1987 on 20.09.19.
//

#ifndef MPI2_OUTPUT_H
#define MPI2_OUTPUT_H

#include "../Application.h"

namespace logging{

    /**
     * Updates the progress bar
     *
     * @param completed number of completed steps
     * @param total number of total steps
     */
    void progress(int completed, int total){
        Application::getInstance().getLoggingEngine().progress(completed, total);
    }

    /**
     * Creates new progress bar
     *
     * @param completed number of completed steps
     * @param total number of total steps
     * @param message message to be printed on the progress bar
     */
    void progress(int completed, int total, const std::string& message){
        Application::getInstance().getLoggingEngine().progress(completed, total, message);
    }

    /**
     * The function shall be put before call of a set of debug()/warning() routines
     */
    void enter(){
        Application::getInstance().getLoggingEngine().enterLog();
    }

    /**
     * The function shall be put after call of a set of debug()/warning() routines
     */
    void exit(){
        Application::getInstance().getLoggingEngine().exitLog();
    }

    /**
     * Prints the debugging message
     *
     * @param msg The debugging message itself
     */
    void debug(const std::string& msg){
        Application::getInstance().getLoggingEngine().debug(msg);
    }
}

#endif //MPI2_OUTPUT_H
