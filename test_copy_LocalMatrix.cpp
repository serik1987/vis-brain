//
// Created by serik1987 on 21.09.19.
//

#include "Application.h"
#include "data/Matrix.h"
#include "data/LocalMatrix.h"
#include "data/ContiguousMatrix.h"

void print_matrix(const data::Matrix& matrix, const std::string& prompt){
    using std::to_string;

    logging::enter();
    logging::debug(prompt);
    matrix.printLocal();
    logging::debug("");
    logging::exit();
}

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

data::LocalMatrix fill_local_matrix(mpi::Communicator& comm, int size, double sizeUm, double filler = 0){
    data::LocalMatrix X(comm, size, size, sizeUm, sizeUm, filler);
    return X;
}

void test_main(){
    using namespace std;

    Application& app = Application::getInstance();
    mpi::Communicator& comm = app.getAppCommunicator();

    logging::progress(0, 5, "Matrix initialization");
    data::LocalMatrix A(comm, 20, 20, 1.0, 1.0);
    fill_matrix(A);
    print_matrix(A, "Matrix A");


    logging::progress(1, 5, "Copy constructor");
    data::LocalMatrix B = A;
    print_matrix(B, "Matrix B");
    fill_matrix(B, 5.5);
    print_matrix(B, "Matrix B after change");
    print_matrix(A, "Matrix A after change");


    logging::progress(2, 5, "Copy operator");
    data::LocalMatrix C(comm, 20, 20, 1.0, 1.0);
    C = A;
    print_matrix(C, "Matrix C");
    fill_matrix(C, 10.8);
    print_matrix(C, "Matrix C after change");
    print_matrix(A, "Matrix A after change");

    logging::progress(3, 5, "Moving constructor");
    data::LocalMatrix D = fill_local_matrix(comm, 20, 1.0, 15.8);
    print_matrix(D, "Matrix D");

    logging::progress(4, 5, "Moving operator");
    data::LocalMatrix E(comm, 20, 20, 1.0, 1.0);
    E = fill_local_matrix(comm, 20, 1.0, 20.4);
    print_matrix(E, "Matrix E");

    logging::progress(5, 5);
}