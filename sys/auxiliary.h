//
// Created by serik1987 on 27.08.19.
//

#ifndef MPI2_AUXILIARY_H
#define MPI2_AUXILIARY_H

#include <string>


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
}


#endif //MPI2_AUXILIARY_H
