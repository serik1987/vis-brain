//
// Created by serik1987 on 15.10.2019.
//

#include "BinStream.h"

namespace data::stream {

    static const int CHUNK_SIZE = 256;
    static const MPI_Offset FRAME_NNUMBER_POSITION = 264;
    static const char CHUNK[CHUNK_SIZE] = "#!vis-brain.data.stream";

    BinStream::BinStream(data::Matrix *matrix, const std::string &filename, data::stream::Stream::StreamMode mode,
                         double sampleRate, bool autoopen): Stream(matrix, filename, mode, sampleRate, autoopen) {
        if (autoopen){
            open();
        }
    }

    void BinStream::openStreamFile() {
        logging::enter();
        if (Application::getInstance().getAppCommunicator().getRank() == 0) {
            logging::debug("STREAM FILE OPENED");
        }
        logging::exit();
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
        logging::enter();
        if (Application::getInstance().getAppCommunicator().getRank() == 0){
            logging::debug("MATRIX HAS BEEN READ FROM THE STREAM");
        }
        logging::exit();
    }

    void BinStream::finishReading() {
        logging::enter();
        if (Application::getInstance().getAppCommunicator().getRank() == 0){
            logging::debug("READING STREAM HAS BEEN CLOSED");
        }
        logging::exit();
    }

    void BinStream::finishWriting() {
        std::exception* exception = nullptr;
        try{
            handle->seek(FRAME_NNUMBER_POSITION, MPI_SEEK_SET);
            handle->writeAll(&frameNumber, 1, MPI_INTEGER);
        } catch (std::exception& e){
            exception = &e;
        }
        delete handle;
        if (exception != nullptr){
            throw *exception;
        }
    }

}