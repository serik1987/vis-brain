//
// Created by serik1987 on 27.08.19.
//

#ifndef MPI2_AUXILIARY_H
#define MPI2_AUXILIARY_H

#include <string>


namespace sys{
    void create_empty_file(const std::string& filename);
    void create_empty_dir(const std::string& pathname);
    std::string get_file_path(const std::string& filename);
    std::string get_home_directory();
    void create_dir(const std::string& dirname);
}


#endif //MPI2_AUXILIARY_H
