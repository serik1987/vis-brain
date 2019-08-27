//
// Created by serik1987 on 12.07.19.
//

#ifndef MPI2_SYS_EXCEPTIONS_H
#define MPI2_SYS_EXCEPTIONS_H

#include <exception>
#include <errno.h>

namespace sys{

    class exception: public std::exception{};

    class access_error: public exception{
        const char* what() const noexcept override{
            return "permission denied";
        }
    };

    class bad_file_error: public exception{
        const char* what() const noexcept override{
            return "invalid file descriptor";
        }
    };

    class other_error: public exception{
        const char* what() const noexcept override{
            return "undocumented error on POSIX request";
        }
    };

    class file_exists_error: public exception{
        const char* what() const noexcept override{
            return "the file has already existed";
        }
    };

    class invalid_argument: public exception{
        const char* what() const noexcept override{
            return "invalid value";
        }
    };

    class too_many_open_files: public exception{
        const char* what() const noexcept override{
            return "too many opened files";
        }
    };

    class too_many_symbolic_links: public exception{
        const char* what() const noexcept override{
            return "too many symbolic links encountered while traversing the path";
        }
    };

    class filename_too_long: public exception{
        const char* what() const noexcept override{
            return "Filename too long";
        }
    };

    class file_not_exists: public exception{
        const char* what() const noexcept override{
            return "file not exists";
        }
    };

    class insufficient_memory: public exception{
        const char* what() const noexcept override{
            return "insufficient memory";
        }
    };

    class interrupted_error: public exception{
        const char* what() const noexcept override{
            return "The call was interrupted by the signal handler";
        }
    };

    class timeout_error: public exception{
        const char* what() const noexcept override{
            return "timeout";
        }
    };

    class overflow_error: public exception{
        const char* what() const noexcept override{
            return "the value is too large or maximum limit exceeded";
        }
    };

    class directory_not_exists: public exception{
        const char* what() const noexcept override{
            return "A component of the path prefix of pathname is not a directory";
        }
    };

    void create_exception();

}

#endif //MPI2_EXCEPTIONS_H
