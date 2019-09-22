//
// Created by serik1987 on 21.09.19.
//

#include "ContiguousMatrix.h"

namespace data{

    ContiguousMatrix::ContiguousMatrix(mpi::Communicator &comm, int width, int height, double widthUm, double heightUm,
                                       double filler):
                                       Matrix(comm, width, height, widthUm, heightUm, filler){
        allocatorSize = ceil((double)size/comm.getProcessorNumber());
        bigData = new double[allocatorSize * comm.getProcessorNumber()];
        synchronizationBuffer = new double[allocatorSize * comm.getProcessorNumber()];
        data = bigData + iStart;
        for (int i=0; i < size; i++){
            bigData[i] = filler;
        }
    }

    ContiguousMatrix::~ContiguousMatrix() {
        delete [] bigData;
        delete [] synchronizationBuffer;
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

}