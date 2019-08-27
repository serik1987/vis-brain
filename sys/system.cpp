//
// Created by serik1987 on 12.07.19.
//

#include "exceptions.h"

namespace sys {

    void create_exception() {
        switch (errno) {
            case EACCES:
                throw access_error();
            case EBADF:
                throw bad_file_error();
            case EEXIST:
                throw file_exists_error();
            case EFAULT:
                throw std::bad_alloc();
            case EINVAL:
                throw invalid_argument();
            case ELOOP:
                throw too_many_symbolic_links();
            case ENFILE:
            case EMFILE:
                throw too_many_open_files();
            case ENAMETOOLONG:
                throw filename_too_long();
            case ENOENT:
                throw file_not_exists();
            case ENOMEM:
                throw insufficient_memory();
            case EINTR:
                throw interrupted_error();
            case ENOTDIR:
                throw directory_not_exists();
            case ETIMEDOUT:
                throw timeout_error();
            case EOVERFLOW:
                throw overflow_error();

            default:
                throw other_error();
        }
    }

}