//
// Created by serik1987 on 27.08.19.
//

#ifndef MPI2_AUXILIARY_H
#define MPI2_AUXILIARY_H

#include <string>
#include <sys/stat.h>
#include <unistd.h>
#include "exceptions.h"


namespace sys{

    /**
     * Creates an empty file and all directories mentioned in the path to it but didn't created
     *
     * @param filename absolute or relative path to the file (don't use ~ as a home dir)
     */
    void create_empty_file(const std::string& filename);

    /**
     * Creates an empty directory and all parent directories that don't exist
     *
     * @param pathname absolute or relative path to the directory
     */
    void create_empty_dir(const std::string& pathname);

    /**
     * returns path of the filename containing a folder where the file is located
     *
     * @param filename the file itself
     * @return the path or '.' if filename is given without the path
     */
    std::string get_file_path(const std::string& filename);

    /**
     * Returns the absolute path to the home directory
     *
     * @return
     */
    std::string get_home_directory();

    /**
     *
     * @param dirname name of the directory
     */
    void create_dir(const std::string& dirname);

    /**
     * Checks for file existence
     *
     * @param filename name of the file to check (absolute or relative path)
     * @return true if the file exists, false if the file doesn't exist or can't be stat()'ed
     */
    inline bool file_exists(const std::string& filename){
        struct stat buf;
        int result = stat(filename.c_str(), &buf);
        errno = 0;
        return (result == 0);
    }

    /**
     *
     * @return name of the current directory
     */
    inline std::string get_current_directory(){
        constexpr int dir_name_size = 256;
        char dirname[dir_name_size];
        if (getcwd(dirname, dir_name_size) == NULL){
            create_exception();
        }
        return dirname;
    }
}


#endif //MPI2_AUXILIARY_H
