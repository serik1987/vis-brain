//
// Created by serik1987 on 20.09.19.
//

#ifndef MPI2_EXCEPTIONS_H
#define MPI2_EXCEPTIONS_H

#include <exception>

class log_exception: public std::exception {};

class system_logger_already_exists: public log_exception{
    const char* what() const noexcept override{
        return "Attempt to create more than one instance of the system logger";
    }
};

#endif //MPI2_EXCEPTIONS_H
