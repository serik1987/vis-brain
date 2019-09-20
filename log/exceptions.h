//
// Created by serik1987 on 20.09.19.
//

#ifndef MPI2_LOGEXCEPTIONS_H
#define MPI2_LOGEXCEPTIONS_H

#include <exception>

class log_exception: public std::exception {};

class system_logger_already_exists: public log_exception{
    const char* what() const noexcept override{
        return "Attempt to create more than one instance of the system logger";
    }
};

class debug_logger_not_enveloped: public log_exception{
    const char* what() const noexcept override{
        return "You made a call of either logging::debug or logging::Logger::debug which is not enveloped by logging::enter and logging::exit";
    }
};

class simulation_exception: public std::exception {};

class default_exception: public simulation_exception{
    const char* what() const noexcept override{
        return "This is to test error generation";
    }
};

#endif //MPI2_LOGEXCEPTIONS_H
