//
// Created by serik1987 on 12.10.2019.
//

#include "Saver.h"

namespace data::reader{

    void Saver::save(data::Matrix &matrix, std::string filename) {
        std::string fullfile = Application::getInstance().getOutputFolder();
        if (fullfile.substr(fullfile.size()-1) != "/"){
            fullfile += "/";
        }
        std::string current_file;
        if (filename == ""){
            current_file = getFilename();
        } else {
            current_file = filename;
        }
        if (current_file.substr(0, 1) ==  "/"){ // absolute path is given
            fullfile = current_file;
        } else { // relative file is given
            fullfile += current_file;
        }
        savingFile = new mpi::File(matrix.getCommunicator(), fullfile,
                                   MPI_MODE_WRONLY | MPI_MODE_CREATE | MPI_MODE_EXCL);
        write(matrix);
        delete savingFile;
    }

}