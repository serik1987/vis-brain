//
// (C) Sergei Kozhukhov, 2019
// (C) the Institute of Higher Nervous Activity and Neurophysiology, Russian Academy of Sciences, 2019
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

    class square_matrix_required: public simulation_exception{
    public:
        const char* what() const noexcept override{
            return "Square matrix is required to complete the current simulation but rectangular matrix was passed";
        }
    };

    class matrix_dimensions_mismatch: public simulation_exception{
    public:
        const char* what() const noexcept override{
            return "Dimensions of different input matrices in the equation are mismatched";
        }
    };

    class incorrect_data_format: public std::exception{
    public:
        const char* what() const noexcept override{
            return "Incorrect file is given as an input file";
        }
    };

    class stream_not_opened: public simulation_exception{
    public:
        const char* what() const noexcept override{
            return "Can't read the matrix from the stream because the stream has not been opened";
        }
    };

    class end_of_stream_reached: public simulation_exception{
    public:
        const char* what() const noexcept override{
            return "The simulation process requires extra frames that are absent in the input stream";
        }
    };

    class external_stream_error: public std::exception{
    public:
        const char* what() const noexcept override{
            return "Attempt to write the local matrix into external stream: only contiguous synchronized matrices can be written";
        }
    };

    class write_only_stream_error: public std::exception{
    public:
        const char* what() const noexcept override {
            return "Can't read from this type of stream. Choose another stream to read";
        }
    };

}

#endif //MPI2_EXCEPTIONS_H
