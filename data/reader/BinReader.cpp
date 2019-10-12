//
// Created by serik1987 on 12.10.2019.
//

#include "BinReader.h"

namespace data::reader{

    static const int CHUNK_SIZE = 256;
    static const char CHUNK[CHUNK_SIZE] = "#!vis-brain.data.reader";

    void BinReader::write(data::Matrix &matrix) {
        savingFile->writeAll(CHUNK, CHUNK_SIZE, MPI_CHAR);
        int dimensions[2] = {matrix.getWidth(), matrix.getHeight()};
        savingFile->writeAll(dimensions, 2, MPI_INT);
        double nativeDimensions[2] = {matrix.getWidthUm(), matrix.getHeightUm()};
        savingFile->writeAll(nativeDimensions, 2, MPI_DOUBLE);
        auto iterator = matrix.begin();
        double* handle = &(*iterator);
        MPI_Offset offset = matrix.getIstart() * sizeof(double);
        int size = matrix.getLocalSize();
        savingFile->seek(offset, MPI_SEEK_CUR);
        savingFile->write(handle, size, MPI_DOUBLE);
    }

}