//
// Created by serik1987 on 28.08.19.
//

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>

class ApplicationError: public std::exception {};

class ApplicationAlreadyExists: public ApplicationError {
public:
    const char* what() const noexcept override{
        return "Application has already been run\n";
    }
};

class UnknownMpiConfiguration: public ApplicationError {
public:
    const char* what() const noexcept override{
        return "Configuration mode given in world.application.configuration_mode is wrong or unsupported\n";
    }
};

#endif //EXCEPTIONS_H
