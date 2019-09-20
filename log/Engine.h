//
// Created by serik1987 on 20.09.19.
//

#ifndef MPI2_LOGENGINE_H
#define MPI2_LOGENGINE_H

#include <string>
#include "SystemLogger.h"
#include "../compile_options.h"
#include "UserLogger.h"

namespace logging {

    /**
     * Represents centralized system for writing log messages of the application
     */
    class Engine {
    private:
        static bool created;
        int steps_completed, steps_total;
        std::string notice_message;

        void hideNotification() const;
        void showNotification(int completed, int total, const std::string& message) {
            notice_message = message;
            showNotification(completed, total);
        };
        void showNotification(int completed, int total) {
            steps_completed = completed;
            steps_total = total;
            showNotification();
        };
        void showNotification() const;

        void logProgress();


        SystemLogger* systemLogger;
#if DEBUG == 1
        UserLogger* publicDebugLogger;
        UserLogger* privateDebugLogger;
#endif
        UserLogger* infoLogger;
        UserLogger* noticeLogger;
        UserLogger* warningLogger;

    public:

        /**
         *
         * @param output_dir - directory where all data shall be saved
         */
        Engine(const std::string& output_dir);
        Engine(const Engine& other) = delete;
        Engine& operator=(const Engine& other) = delete;
        ~Engine();

        static char green[];
        static char yellow[];
        static char magenta[];
        static char red[];
        static char reset[];


        /**
         * Prints new progress bar on the screen and writes logs about the progress
         *
         * @param completed number of completed steps
         * @param total total number of steps
         * @param message Message to show
         */
        void progress(int completed, int total, const std::string& message);

        /**
         * Updates current progress bar on the screen and writes logs about the progress
         *
         * @param completed number of completed steps
         * @param total total number of steps
         */
        void progress(int completed, int total);


        /**
         * Prints the debug message
         * The function will print the debug message correctly when you can enterLog() method before and
         * exitDLog() method after. You can call several debug(...) methods enveloped by enterDebug() and exitDebug()
         * Also, don't forget to insert #if DEBUG==1 line before and #endif line after if you want your application
         * to be compiled successfully
         *
         * @param msg Message to print
         */
        void debug(const std::string& msg);

        /**
         * Shall be run before printing the debug messages
         */
        void enterLog();

        /**
         * Shall be run after printing the debug messages
         */
        void exitLog();
    };

}


#endif //MPI2_LOGENGINE_H
