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

data::ContiguousMatrix fill_contiguous_matrix(mpi::Communicator& comm, int size, double sizeUm, double filler = 0){
    data::ContiguousMatrix X(comm, size, size, sizeUm, sizeUm, filler);
    return X;
}

void test_main(){
    using namespace std;

    Application& app = Application::getInstance();
    mpi::Communicator& comm = app.getAppCommunicator();

    logging::progress(0, 6, "Matrix initialization");
    data::ContiguousMatrix A(comm, 20, 20, 1.0, 1.0);
    fill_matrix(A);
    print_matrix(A, "Matrix A");
    A.synchronize();
    print_matrix(A, "Matrix A, after synchronization");

    logging::progress(1, 6, "Copy constructor");
    data::ContiguousMatrix B = A;
    print_matrix(B, "Matrix B, just created");
    fill_matrix(B, 5.8);
    print_matrix(B, "Matrix B, change the data");
    print_matrix(A, "Matrix A, when somebody changes the data in the matrix B");
    B.synchronize();
    print_matrix(B, "Matrix B, after synchronization");

    logging::progress(2, 6, "Copy operator");
    data::ContiguousMatrix C(comm, 30, 30, 2.0, 2.0);
    C = A;
    print_matrix(C, "Matrix C, after assignment");
    fill_matrix(C, 10.4);
    print_matrix(C, "Matrix C, after change");
    print_matrix(A, "Matrix A, after matrix C was changed");
    C.synchronize();
    print_matrix(C, "Matrix C, synchronized");

    logging::progress(3, 6, "Copy operator, same size");
    data::ContiguousMatrix D(comm, 20, 20, 1.0, 1.0);
    D = A;
    print_matrix(D, "Matrix D, after assignment");
    fill_matrix(D, 15.6);
    print_matrix(D, "Matrix D, after change");
    print_matrix(A, "Matrix A, after matrix D changed");
    D.synchronize();
    print_matrix(D, "Matrix D, synchronized");

    logging::progress(4, 6, "Move constructor");
    data::ContiguousMatrix E = fill_contiguous_matrix(comm, 20, 1.0, 25.4);
    print_matrix(E, "Matrix E, after initialization");
    E.synchronize();
    print_matrix(E, "Matrix E, after synchronization");

    logging::progress(5, 6, "Move operator");
    data::ContiguousMatrix F(comm, 30, 30, 1.5, 1.5);
    F = fill_contiguous_matrix(comm, 20, 1.5, 30.6);
    print_matrix(F, "Matrix F, after initialization");
    F.synchronize();
    print_matrix(F, "Matrix F, after synchronization");


    logging::progress(6, 6);
}