//
// Created by serik1987 on 12.10.2019.
//

#include "Loader.h"

namespace data::reader{

    LocalMatrix* Loader::load(mpi::Communicator& comm, const std::string &filename) {
        std::string current_filename;
        if (filename == ""){
            current_filename = getFilename();
        } else {
            current_filename = filename;
        }
        LocalMatrix* result = loadFile(comm, current_filename);
        return result;
    }

    LocalMatrix* Loader::loadFile(mpi::Communicator &comm, const std::string &current_filename) {
        file = new mpi::File(comm, current_filename, MPI_MODE_RDONLY);
        data::LocalMatrix* result = read(comm);
        delete file;
        return result;
    }

}