//
// Created by serik1987 on 15.10.2019.
//

#include "BinStream.h"

namespace data::stream {

    static const int CHUNK_SIZE = 256;
    static const MPI_Offset FRAME_NUMBER_POSITION = 264;
    static const char CHUNK[CHUNK_SIZE] = "#!vis-brain.data.stream";

    BinStream::BinStream(data::Matrix *matrix, const std::string &filename, data::stream::Stream::StreamMode mode,
                         double sampleRate, bool autoopen): Stream(matrix, filename, mode, sampleRate, autoopen) {
        if (autoopen){
            open();
        }
    }

    void BinStream::openStreamFile() {
        handle = new mpi::File(getCommunicator(), filename, MPI_MODE_RDONLY);
        try{
            char actual_chunk[CHUNK_SIZE];
            handle->readAll(actual_chunk, CHUNK_SIZE, MPI_CHAR);
            if (strcmp(actual_chunk, CHUNK) != 0){
                throw incorrect_data_format();
            }
            int integer_headers[3];
            handle->readAll(integer_headers, 3, MPI_INTEGER);
            int height = integer_headers[0];
            int width = integer_headers[1];
            totalFrames = integer_headers[2];
            if (height != getMatrix().getHeight() || width != getMatrix().getWidth()){
                throw matrix_dimensions_mismatch();
            }
            double float_headers[3];
            handle->readAll(float_headers, 3, MPI_DOUBLE);
            sampleRate = float_headers[2];
        } catch (std::exception& exc){
            delete handle;
            throw;
        }
    }

    void BinStream::createStreamFile(){
        handle = new mpi::File(getCommunicator(), filename,
                MPI_MODE_WRONLY | MPI_MODE_CREATE | MPI_MODE_EXCL);
        try{
            handle->writeAll(CHUNK, CHUNK_SIZE, MPI_CHAR);
            int header_integer[3] = {getMatrix().getHeight(), getMatrix().getWidth(), frameNumber};
            handle->writeAll(header_integer, 3, MPI_INTEGER);
            double header_double[3] = {getMatrix().getHeightUm(), getMatrix().getWidthUm(), getSampleRate()};
            handle->writeAll(header_double, 3, MPI_DOUBLE);
        } catch (std::exception& e){
            delete handle;
            throw;
        }
    }

    void BinStream::writeMatrix(data::Matrix *matrix) {
        auto a = matrix->begin();
        double* buffer = &(*a);
        if (frameNumber == 0) {
            handle->seek(matrix->getIstart() * sizeof(double), MPI_SEEK_CUR);
        } else {
            handle->seek((matrix->getSize() - matrix->getLocalSize()) * sizeof(double), MPI_SEEK_CUR);
        }
        handle->write(buffer, matrix->getLocalSize(), MPI_DOUBLE);
    }

    void BinStream::readMatrix(data::Matrix *matrix) {
        auto a = matrix->begin();
        double* buffer = &(*a);
        if (frameNumber == 0){
            handle->seek(matrix->getIstart() * sizeof(double), MPI_SEEK_CUR);
        } else {
            handle->seek((matrix->getSize() - matrix->getLocalSize()) * sizeof(double), MPI_SEEK_CUR);
        }
        handle->read(buffer, matrix->getLocalSize(), MPI_DOUBLE);
    }

    void BinStream::finishReading() {
        delete handle;
    }

    void BinStream::finishWriting() {
        try{
            handle->seek(FRAME_NUMBER_POSITION, MPI_SEEK_SET);
            handle->writeAll(&frameNumber, 1, MPI_INTEGER);
        } catch (std::exception& e){
            delete handle;
            throw;
        }
        delete handle;
    }

}