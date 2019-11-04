//
// Created by serik1987 on 20.09.19.
//

#ifndef MPI2_OUTPUT_H
#define MPI2_OUTPUT_H

#include "../Application.h"

namespace logging{

    /**
     * Updates the progress bar. The function will be executed only by the process with rank # 0
     *
     * @param completed number of completed steps
     * @param total number of total steps
     */
    inline void progress(int completed, int total){
        Application::getInstance().getLoggingEngine().progress(completed, total);
    }

    /**
     * Creates new progress bar. The function will be executed only by the process with rank 0
     *
     * @param completed number of completed steps
     * @param total number of total steps
     * @param message message to be printed on the progress bar
     */
    inline void progress(int completed, int total, const std::string& message){
        Application::getInstance().getLoggingEngine().progress(completed, total, message);
    }

    /**
     * The function shall be put before call of a set of debug()/warning() routines
     * Collective routine
     */
    inline void enter(){
        Application::getInstance().getLoggingEngine().enterLog();
    }

    /**
     * The function shall be put after call of a set of debug()/warning() routines
     * Collective routine
     */
    inline void exit(){
        Application::getInstance().getLoggingEngine().exitLog();
    }

#if DEBUG==1
    /**
     * Prints the debugging message
     * Collective routine
     *
     * @param msg The debugging message itself
     */
    inline void debug(const std::string& msg){
        Application::getInstance().getLoggingEngine().debug(msg);
    }
#endif

    /**
     * Prints information about the simulation parameters to the log file. The information will be printed only
     * by the process with rank 0
     *
     * @param msg string containing such information
     */
    inline void info(const std::string& msg){
        Application::getInstance().getLoggingEngine().info(msg);
    }

    /**
     * Prints warning.
     *
     * Ways to print warning
     * Way # 1. Printing warning to the console as well as to .log files; collective routine
     * logging::enter(); // This is a collective routine
     * logging::warning(msg); // Useless for processes with rank different from zero
     * logging::exit(); // This is collective routine
     *
     * Way # 2. Printing warning to the .log files but not to screen console
     * logging::warning(msg); // Useless for any processes with rank different from zero
     *
     * Way # 3. Printing warning by the process with k-th rank
     * if (Application::getInstance()->getAppCommunicator()->getRank() == k){
     *      logging::warning(msg, false)
     * }
     *
     * @param msg Message to warn
     * @param print_as_root true if the message shall be printed by the process with rank 0, false otherwise
     */
    inline void warning(const std::string& msg, bool print_as_root = true){
        Application::getInstance().getLoggingEngine().warning(msg, print_as_root);
    }


    /**
     * Generates error message
     * The routine has effect only for a process with rank = 0
     *
     * @param e an exception thrown
     */
    inline void error(const simulation_exception& e){
        Application::getInstance().getLoggingEngine().error(e);
    }
}

#endif //MPI2_OUTPUT_H
