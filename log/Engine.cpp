//
// Created by serik1987 on 20.09.19.
//

#include <iostream>
#include <unistd.h>
#include "Engine.h"
#include "exceptions.h"
#include "../Application.h"

namespace logging{

    bool Engine::created = false;
    char Engine::green[] = "\033[32;1m";
    char Engine::yellow[] = "\033[33;1m";
    char Engine::magenta[] = "\033[35;1m";
    char Engine::red[] = "\033[31;1m";
    char Engine::reset[] = "\033[0m";

    Engine::Engine(const std::string& output_dir){
        if (created){
            throw system_logger_already_exists();
        }
        created = true;
        systemLogger = new SystemLogger();
        std::string output_folder = Application::getInstance().getOutputFolder();
#if DEBUG == 1
        publicDebugLogger = new UserLogger(PUBLIC_DEBUG_LOG_FILE);
        privateDebugLogger = new UserLogger(output_folder + PRIVATE_DEBUG_LOG_FILE);
#endif
        infoLogger = new UserLogger(output_folder + MODEL_DESCRIPTION_LOG_FILE);
        noticeLogger = new UserLogger(output_folder + NOTICE_LOG_FILE);
        warningLogger = new UserLogger(output_folder + WARNING_LOG_FILE);
    }

    Engine::~Engine(){
        created = false;
        delete systemLogger;
#if DEBUG == 1
        delete publicDebugLogger;
        delete privateDebugLogger;
#endif
        delete infoLogger;
        if (Application::getInstance().getAppCommunicator().getRank() == 0) {
            noticeLogger->writeLog("DONE", Logger::Notice, false, false);
        }
        delete noticeLogger;
        delete warningLogger;
    }


    void Engine::hideNotification() const {
        std::cerr << "\033[2K\033[0G";
    }

    void Engine::showNotification() const {
        hideNotification();
        std::cerr << notice_message << "...";
        if (steps_completed == steps_total){
            std::cerr << green << "OK" << reset << std::endl;
        } else {
            std::cerr << steps_completed << "/" << steps_total;
        }
    }

    void Engine::progress(int completed, int total, const std::string &message) {
        if (Application::getInstance().getAppCommunicator().getRank() == 0) {
            if (notice_message != "") {
                showNotification(steps_total, steps_total);
            }
            showNotification(completed, total, message);
            logProgress();
        }
    }

    void Engine::progress(int completed, int total){
        if (Application::getInstance().getAppCommunicator().getRank() == 0) {
            hideNotification();
            showNotification(completed, total);
            logProgress();
        }
    }

    void Engine::logProgress(){
        std::ostringstream ss;
        ss << steps_completed << ":" << steps_total << ":" << notice_message;
        noticeLogger->writeLog(ss.str(), Logger::Notice, false, false);
        systemLogger->writeLog(ss.str(), Logger::Notice, false, false);
    }

    void Engine::enterLog() {
        int x;
        Application::getInstance().time();
        Application::getInstance().getAppCommunicator().barrier();
        Application::getInstance().getAppCommunicator().broadcast(&x, 1, MPI_INTEGER, 0);
        if (Application::getInstance().getAppCommunicator().getRank() == 0) {
            hideNotification();
        }
        Application::getInstance().lock();
        enveloped = true;
    }

#if DEBUG==1
    void Engine::debug(const std::string &msg) {
        std::cerr << Logger::makeString(msg, Logger::Debug, false, true);
        publicDebugLogger->writeLog(msg, Logger::Debug, false, true);
        privateDebugLogger->writeLog(msg, Logger::Debug, false, true);
        systemLogger->writeLog(msg, Logger::Debug, false, true);
    }
#endif

    void Engine::info(const std::string& msg){
        if (Application::getInstance().getAppCommunicator().getRank() == 0){
            infoLogger->writeLog(msg, Logger::Info, false, false);
            systemLogger->writeLog(msg, Logger::Info, false, false);
        }
    }

    void Engine::warning(const std::string &msg, bool print_as_root) {
        int rank = Application::getInstance().getAppCommunicator().getRank();
        if (!print_as_root || rank == 0){
            if (enveloped){
                std::cerr << yellow << Logger::makeString(msg, Logger::Warning, true, false) << reset;
            }
            infoLogger->writeLog(msg, Logger::Warning, true, false);
            warningLogger->writeLog(msg, Logger::Warning, true, false);
            systemLogger->writeLog(msg, Logger::Warning, true, false);
        }

    }

    void Engine::exitLog() {
        enveloped = false;
        int* x = new int[Application::getInstance().getAppCommunicator().getProcessorNumber()];
        int y;
        Application::getInstance().unlock();
        std::cerr.flush();
        Application::getInstance().getAppCommunicator().barrier();
        Application::getInstance().getAppCommunicator().gather(&y, 1, MPI_INTEGER, x, 1, MPI_INTEGER, 0);
        if (Application::getInstance().getAppCommunicator().getRank() == 0){
            sleep(0.1);
            showNotification();
        }
        delete [] x;

    }

}