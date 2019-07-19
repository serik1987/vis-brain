//
// Created by User on 30.06.2019.
//

#ifndef MPI2_EXCEPTIONS_H
#define MPI2_EXCEPTIONS_H

#include <exception>
#include "mpi.h"

class app_exception: public std::exception{};

namespace mpi{

    class exception: public app_exception{};

    class already_initialized_exception: public exception{
    public:
        const char* what() const noexcept override{
            return "Application has already initialized";
        }
    };

    class not_initialized_exception: public exception{
    public:
        const char* what() const noexcept override{
            return "Application has not been initialized";
        }
    };

    class unknown_error: public exception{
        const char* what() const noexcept override{
            return "Unfortunately, the exception occured was not recognized.";
        }
    };

    class communicator_error: public exception{
        const char* what() const noexcept override{
            return "Bad or damaged Communicator instance: it refers to invalid communicator.";
        }
    };

    class datatype_error: public exception{
        const char* what() const noexcept override{
            return "Invalid datatype in the MPI_Datatype";
        }
    };

    class bad_pointer_error: public exception{
        const char* what() const noexcept override{
            return "The data pointer refers to the wrong object.";
        }
    };

    class root_error: public exception{
        const char* what() const noexcept override{
            return "Invalid process rank";
        }
    };

    class group_error: public exception{
        const char* what() const noexcept override{
            return "The group is invalid or has already destroyed";
        }
    };

    class internal_error: public exception{
        const char* what() const noexcept override{
            return "Unable to allocate memory to complete this operation";
        }
    };

    class rank_error: public exception{
        const char* what() const noexcept override{
            return "Invalid rank";
        }
    };

    class operation_error: public exception{
        const char* what() const noexcept override{
            return "Invalid operation used during the reduction";
        }
    };

    class topology_error: public exception{
        const char* what() const noexcept override{
            return "No topology associated with communicator";
        }
    };

    class buffer_error: public exception{
        const char* what() const noexcept override{
            return "Invalid buffer or buffer is not attached";
        }
    };

    class request_error: public exception{
        const char* what() const noexcept override{
            return "Invalid request";
        }
    };

    class status_error: public exception{
        const char* what() const noexcept override{
            return "error in MPI status";
        }
    };

    class receive_error: public exception{
        const char* what() const noexcept override{
            return "the message was truncated during the receive";
        }
    };

    class pending_error: public exception{
        const char* what() const noexcept override{
            return "The operation was not completed";
        }
    };

    class keyval_error: public exception{
        const char* what() const noexcept override{
            return "Invalid keyval used";
        }
    };

    class access_denied: public exception{
        const char* what() const noexcept override{
            return "Access denied";
        }
    };

    class bad_file: public exception{
        const char* what() const noexcept override{
            return "invalid file name";
        }
    };

    class dup_datarep_error: public exception{
        const char* what() const noexcept override{
            return "Conversion fulctions could not be registered becase a data representation "
                   "hat has already defined was passed";
        }
    };

    class file_exists_error: public exception{
        const char* what() const noexcept override{
            return "File exists";
        }
    };

    class file_in_use_error: public exception{
        const char* what() const noexcept override{
            return "File operation could not be completed, as the file is currently opened by some process";
        }
    };

    class file_error: public exception{
        const char* what() const noexcept override{
            return "Invalid file handler";
        }
    };

    class io_error: public exception{
        const char* what() const noexcept override{
            return "I/O error";
        }
    };

    class not_enough_space_error: public exception{
        const char* what() const noexcept override{
            return "Not enough space to write the file";
        }
    };

    class file_not_found_error: public exception{
        const char* what() const noexcept override{
            return "File does not exist";
        }
    };

    class read_only_error: public exception{
        const char* what() const noexcept override{
            return "Read only file or file system";
        }
    };

    class unsupported_datarep: public exception{
        const char* what() const noexcept override{
            return "Unsupported datarep";
        }
    };

    class info_error: public exception{
        const char* what() const noexcept override{
            return "Invalid MPI info passed";
        }
    };

    class key_too_long_error: public exception{
        const char* what() const noexcept override{
            return "MPI key is too long";
        }
    };

    class value_too_long_error: public exception{
        const char* what() const noexcept override{
            return "MPI value is too long";
        }
    };

    class nokey_error: public exception{
        const char* what() const noexcept override{
            return "Invalid key passed";
        }
    };

    class service_name_error: public exception{
        const char* what() const noexcept override{
            return "Invalid service name passed in MPI_LOOKUP_NAME";
        }
    };

    class port_name_error: public exception{
        const char* what() const noexcept override{
            return "Invalid port name passed";
        }
    };

    class quota_error: public exception{
        const char* what() const noexcept override{
            return "Quota exceeded";
        }
    };

    class spawning_error: public exception{
        const char* what() const noexcept override{
            return "Spawning error";
        }
    };

    class unsupported_operation_error: public exception{
        const char*what() const noexcept override{
            return "Operation unsupported";
        }
    };

    class file_size_error: public exception{
        const char* what() const noexcept override{
            return "File size error";
        }
    };

    class mpi_info_key_not_found: public exception{
        const char* what() const noexcept override{
            return "Key in MPI info not found";
        }
    };

    void throw_exception(int errcode);
}

#endif //MPI2_EXCEPTIONS_H
