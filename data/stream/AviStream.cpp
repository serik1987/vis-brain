//
// Created by serik1987 on 18.10.2019.
//

#include "AviStream.h"

namespace data::stream {

    void AviStream::createExternalStream() {
        std::cout << "AVI: CREATE EXTERNAL STREAM\n";
    }

    void AviStream::writeContiguousMatrix(data::ContiguousMatrix *matrix) {
        std::cout << "AVI: WRITE CONTIGUOUS MATRIX\n";
    }

    void AviStream::closeExternalWritingStream() {
        std::cout << "AVI: CLOSE EXTERNAL STREAM\n";
    }

}