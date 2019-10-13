//
// Created by serik1987 on 12.10.2019.
//

#include "ExternalSaver.h"
#include "../ContiguousMatrix.h"

namespace data::reader {

    void ExternalSaver::saveFile(data::Matrix &matrix, const std::string &current_filename) {
        buffer = new data::ContiguousMatrix(matrix.getCommunicator(), matrix.getWidth(), matrix.getHeight(),
                matrix.getWidthUm(), matrix.getHeightUm());
        buffer->calculate(matrix, [](auto x){
            return *x;
        });
        buffer->synchronize(root);
        filename = current_filename;
        if (matrix.getCommunicator().getRank() == root){
            write(*buffer);
        }
        delete buffer;
    }

}