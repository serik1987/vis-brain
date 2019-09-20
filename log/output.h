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
}

#endif //MPI2_OUTPUT_H
