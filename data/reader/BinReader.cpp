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

    data::LocalMatrix* BinReader::read(mpi::Communicator& comm){
        char actual_chunk[CHUNK_SIZE];
        file->readAll(actual_chunk, CHUNK_SIZE, MPI_CHAR);
        if (strcmp(actual_chunk, CHUNK) != 0){
            throw incorrect_data_format();
        }
        int dimensions[2];
        int& width = dimensions[0];
        int& height = dimensions[1];
        file->readAll(dimensions, 2, MPI_INT);
        double nativeDimensions[2];
        double& nativeWidth = nativeDimensions[0];
        double& nativeHeight = nativeDimensions[1];
        file->readAll(nativeDimensions, 2, MPI_DOUBLE);
        data::LocalMatrix* matrix = new data::LocalMatrix(comm, width, height, nativeWidth, nativeHeight);
        MPI_Offset offset = matrix->getIstart() * sizeof(double);
        int size = matrix->getLocalSize();
        file->seek(offset, MPI_SEEK_CUR);
        auto it = matrix->begin();
        double* buffer = &(*it);
        file->read(buffer, size, MPI_DOUBLE);
        return matrix;
    }

}