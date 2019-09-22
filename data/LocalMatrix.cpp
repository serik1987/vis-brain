//
// Created by serik1987 on 21.09.19.
//

#include "LocalMatrix.h"


namespace data{

    LocalMatrix::LocalMatrix(mpi::Communicator &comm, int width, int height, double widthUm, double heightUm,
                             double filler): Matrix(comm, width, height, widthUm, heightUm, filler) {
        data = new double[localSize];
        for (int i=0; i < localSize; i++){
            data[i] = filler;
        }
    }

    LocalMatrix::~LocalMatrix() {
        delete [] data;
    }

}