//
// Created by serik1987 on 21.09.19.
//

#include "ContiguousMatrix.h"
#include "../Application.h"

namespace data{

    ContiguousMatrix::ContiguousMatrix(mpi::Communicator &comm, int width, int height, double widthUm, double heightUm,
                                       double filler):
                                       Matrix(comm, width, height, widthUm, heightUm, filler){
        createBuffers();
        for (int i=0; i < size; i++){
            bigData[i] = filler;
        }
    }

    void ContiguousMatrix::createBuffers(){
        allocatorSize = ceil((double)size/communicator.getProcessorNumber());
        bigData = new double[allocatorSize * communicator.getProcessorNumber()];
        synchronizationBuffer = new double[allocatorSize * communicator.getProcessorNumber()];
        data = bigData + iStart;
    }

    void ContiguousMatrix::deleteBuffers(){
        if (bigData != nullptr) {
            delete[] bigData;
            delete[] synchronizationBuffer;
            bigData = nullptr;
            synchronizationBuffer = nullptr;
        }
    }

    ContiguousMatrix::ContiguousMatrix(const ContiguousMatrix &other):
        Matrix(other.communicator, other.width, other.height, other.widthUm, other.heightUm){
        createBuffers();
        memcpy(data, other.data, localSize * sizeof(double));
    }

    ContiguousMatrix::~ContiguousMatrix() {
        deleteBuffers();
    }

    void ContiguousMatrix::synchronize(int root) {
        double* temporaryBuffer;
        communicator.gather(data, allocatorSize, MPI_DOUBLE, synchronizationBuffer, allocatorSize, MPI_DOUBLE, root);
        if (rank == root) {
            temporaryBuffer = bigData;
            bigData = synchronizationBuffer;
            synchronizationBuffer = temporaryBuffer;
            data = bigData + iStart;
        }
    }

    void ContiguousMatrix::synchronize() {
        double* temporaryBuffer;
        communicator.allGather(data, allocatorSize, MPI_DOUBLE, synchronizationBuffer, allocatorSize, MPI_DOUBLE);
        temporaryBuffer = bigData;
        bigData = synchronizationBuffer;
        synchronizationBuffer = temporaryBuffer;
        data = bigData + iStart;
    }

    ContiguousMatrix& ContiguousMatrix::operator=(const ContiguousMatrix& other){
        if (width == other.width && height == other.height &&
            communicator.getProcessorNumber() == other.communicator.getProcessorNumber()){
            int nprocs = communicator.getProcessorNumber();
            localSize = ceil((double) size / nprocs);
            iStart = localSize * rank;
            iFinish = iStart + localSize;
            if (iFinish > size) {
                iFinish = size;
                localSize = iFinish - iStart;
            }
            data = bigData + iStart;
        } else {
            size = other.size;
            width = other.width;
            height = other.height;
            widthUm = other.widthUm;
            heightUm = other.heightUm;
            rank = communicator.getRank();
            int nprocs = communicator.getProcessorNumber();
            localSize = ceil((double) size / nprocs);
            iStart = localSize * rank;
            iFinish = iStart + localSize;
            if (iFinish > size) {
                iFinish = size;
                localSize = iFinish - iStart;
            }
            deleteBuffers();
            createBuffers();
        }
        memcpy(data, other.data, localSize * sizeof(double));
        return *this;
    }

    ContiguousMatrix& ContiguousMatrix::operator=(ContiguousMatrix&& other){
        if (communicator.getProcessorNumber() != other.communicator.getProcessorNumber()){
            throw matrix_move_error();
        }
        deleteBuffers();
        width = other.width;
        height = other.height;
        widthUm = other.widthUm;
        heightUm = other.heightUm;
        size = other.size;
        localSize = other.localSize;
        rank = other.rank;
        iStart = other.iStart;
        iFinish = other.iFinish;
        allocatorSize = other.allocatorSize;
        data = other.data;
        other.data = nullptr;
        bigData = other.bigData;
        other.bigData = nullptr;
        synchronizationBuffer = other.synchronizationBuffer;
        other.synchronizationBuffer = nullptr;
        return *this;
    }

}