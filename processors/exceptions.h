//
// Created by serik1987 on 12.11.2019.
//

#ifndef MPI2_PROCESSORS_EXCEPTIONS_H
#define MPI2_PROCESSORS_EXCEPTIONS_H

#include "../param/exceptions.h"

class uninitialized_processor: public simulation_exception{
    const char* what() const noexcept override{
        return "Trying to access output data of the unitialized processor";
    }
};

#endif //MPI2_PROCESSORS_EXCEPTIONS_H
