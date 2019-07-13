#include "exceptions.h"

#include <stdexcept>
#include <iostream>

namespace mpi{

    void throw_exception(int errcode){
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
            case MPI_ERR_KEYVAL:
            case MPI_KEYVAL_INVALID: throw keyval_error();
            case MPI_ERR_LASTCODE: throw exception();
            case MPI_ERR_ACCESS: throw access_denied();
            case MPI_ERR_AMODE: throw std::invalid_argument("invalid amode chosen");
            case MPI_ERR_BAD_FILE: throw bad_file();
            case MPI_ERR_DUP_DATAREP: throw dup_datarep_error();
            case MPI_ERR_FILE_EXISTS: throw file_exists_error();
            case MPI_ERR_FILE_IN_USE: throw file_in_use_error();
            case MPI_ERR_FILE: throw file_error();
            case MPI_ERR_IO: throw io_error();
            case MPI_ERR_NO_SPACE: throw not_enough_space_error();
            case MPI_ERR_NO_SUCH_FILE: throw file_not_found_error();
            case MPI_ERR_READ_ONLY: throw read_only_error();
            case MPI_ERR_UNSUPPORTED_DATAREP: throw unsupported_datarep();
            case MPI_ERR_INFO: throw info_error();
            case MPI_ERR_INFO_KEY: throw key_too_long_error();
            case MPI_ERR_INFO_VALUE: throw value_too_long_error();
            case MPI_ERR_INFO_NOKEY: throw nokey_error();
            case MPI_ERR_SERVICE:
            case MPI_ERR_NAME: throw service_name_error();
            case MPI_ERR_NO_MEM: throw std::bad_alloc();
            case MPI_ERR_NOT_SAME: throw std::invalid_argument("collective argument not identical on all processes"
                                                               " or collective routines called in different order"
                                                               " by different processes");
            case MPI_ERR_PORT: throw port_name_error();
            case MPI_ERR_QUOTA: throw quota_error();
            case MPI_ERR_SPAWN: throw spawning_error();
            case MPI_ERR_UNSUPPORTED_OPERATION: throw unsupported_operation_error();
            case MPI_ERR_SIZE: throw file_size_error();
            case MPI_ERR_DISP: throw std::invalid_argument("invalid disp argument");


            default:
                std::cerr << "[ERROR] unknown error\n";
                throw exception();
        }
    }

}