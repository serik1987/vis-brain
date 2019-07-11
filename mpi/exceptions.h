//
// Created by User on 30.06.2019.
//

#ifndef MPI2_EXCEPTIONS_H
#define MPI2_EXCEPTIONS_H

#include "mpi.h"

class app_exception: public std::exception{};

namespace mpi{

    class exception: public app_exception{};

    class already_initialized_exception: public exception{
    public:
        const char* what(){
            return "Application has already initialized";
        }
    };

    class not_initialized_exception: public exception{
    public:
        const char* what() {
            return "Application has not been initialized";
        }
    };

    class unknown_error: public exception{
        const char* what(){
            return "Unfortunately, the exception occured was not recognized.";
        }
    };

    class communicator_error: public exception{
        const char* what(){
            return "Bad or damaged Communicator instance: it refers to invalid communicator.";
        }
    };

    class datatype_error: public exception{
        const char* what(){
            return "Invalid datatype in the MPI_Datatype";
        }
    };

    class bad_pointer_error: public exception{
        const char* what(){
            return "The data pointer refers to the wrong object.";
        }
    };

    class root_error: public exception{
        const char* what(){
            return "Invalid process rank";
        }
    };

    class group_error: public exception{
        const char* what(){
            return "The group is invalid or has already destroyed";
        }
    };

    class internal_error: public exception{
        const char* what(){
            return "Unable to allocate memory to complete this operation";
        }
    };

    class rank_error: public exception{
        const char* what(){
            return "Invalid rank";
        }
    };

    class operation_error: public exception{
        const char* what(){
            return "Invalid operation used during the reduction";
        }
    };

    class topology_error: public exception{
        const char* what(){
            return "No topology associated with communicator";
        }
    };

    class buffer_error: public exception{
        const char* what(){
            return "Invalid buffer or buffer is not attached";
        }
    };

    class request_error: public exception{
        const char* what(){
            return "Invalid request";
        }
    };

    class status_error: public exception{
        const char* what(){
            return "error in MPI status";
        }
    };

    class receive_error: public exception{
        const char* what(){
            return "the message was truncated during the receive";
        }
    };

    class pending_error: public exception{
        const char* what(){
            return "The operation was not completed";
        }
    };

    class keyval_error: public exception{
        const char* what(){
            return "Invalid keyval used";
        }
    };

    inline void throw_exception(int errcode){
        switch (errcode){
            case MPI_ERR_BUFFER: throw buffer_error();
            case MPI_ERR_COUNT: throw std::invalid_argument("invalid count in MPI routine");
            case MPI_ERR_TYPE: throw datatype_error();
            case MPI_ERR_TAG: throw std::invalid_argument("invalid tag in MPI routine");
            case MPI_ERR_COMM: throw communicator_error();
            case MPI_ERR_RANK: throw rank_error();
            case MPI_ERR_REQUEST: throw request_error();
            case MPI_ERR_ROOT: throw root_error();
            case MPI_ERR_GROUP: throw group_error();
            case MPI_ERR_OP: throw operation_error();
            case MPI_ERR_TOPOLOGY: throw topology_error();
            case MPI_ERR_DIMS: throw std::invalid_argument("invalid dimensions in MPI routine");
            case MPI_ERR_ARG: throw std::invalid_argument("invalid argument was given to MPI routine");
            case MPI_ERR_UNKNOWN: throw unknown_error();
            case MPI_ERR_TRUNCATE: throw receive_error();
            case MPI_ERR_OTHER: throw exception();
            case MPI_ERR_INTERN: throw internal_error();
            case MPI_ERR_IN_STATUS: throw status_error();
            case MPI_ERR_PENDING: throw pending_error();
            case MPI_KEYVAL_INVALID: throw keyval_error();
            case MPI_ERR_LASTCODE: throw exception();
            default: throw exception();
        }
    }
}

#endif //MPI2_EXCEPTIONS_H
