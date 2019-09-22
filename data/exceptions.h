//
// Created by serik1987 on 21.09.19.
//

#ifndef MPI2_EXCEPTIONS_H
#define MPI2_EXCEPTIONS_H

#include "../log/exceptions.h"

namespace data{

    class missed_data_error: public simulation_exception{
    public:
        const char* what() const noexcept override{
            return "Attempt to access an element of the local matrix that is out of the process responsibility zone";
        }
    };

    class out_of_range_error: public simulation_exception{
    public:
        const char* what() const noexcept override{
            return "Attempt to access an element that is out of range";
        }
    };

    class synchronization_error: public simulation_exception{
    public:
        const char* what() const noexcept override{
            return "Matrix synchronization is not supported for local or sparse matrices";
        }
    };

    class matrix_move_error: public simulation_exception{
    public:
        const char* what() const noexcept override{
            return "Can't move the matrix working under different communicator";
        }
    };

}

#endif //MPI2_EXCEPTIONS_H
