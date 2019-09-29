//
// Created by serik1987 on 21.09.19.
//

#include "Application.h"
#include "data/Matrix.h"
#include "data/LocalMatrix.h"
#include "data/ContiguousMatrix.h"

void fill_matrix(data::Matrix& matrix, double filler = 0){
    for (int i=0; i < matrix.getHeight(); i++){
        for (int j=0; j < matrix.getWidth(); j++){
            int index = i * matrix.getWidth() + j;
            if (index >= matrix.getIstart() && index < matrix.getIfinish()){
                if (filler == 0) {
                    matrix.getValue(i, j) = 100 * i + j;
                } else {
                    matrix.getValue(i, j) = filler;
                }
            }
        }
    }
}

void test_main(){
    using namespace std;

    logging::progress(0, 1, "Matrix initialization");
    mpi::Communicator& comm = Application::getInstance().getAppCommunicator();
    data::LocalMatrix A(comm, 21, 21, 1.0, 1.0);
    data::ContiguousMatrix B(comm, 21, 21, 1.0, 1.0, 3.8);
    fill_matrix(A);
    logging::enter();
    logging::debug("Matrix A, after initialization");
    A.printLocal();
    logging::debug("Matrix B, after initialization");
    B.printLocal();
    logging::exit();

    ::swap(A, B);

    logging::enter();
    logging::debug("Matrix A, after swapping");
    A.printLocal();
    logging::debug("Matrix B, after swapping");
    B.printLocal();
    logging::exit();

    B.synchronize();

    logging::enter();
    logging::debug("Matrix B, after synchronization");
    B.printLocal();
    logging::exit();



    logging::progress(1, 1);
}