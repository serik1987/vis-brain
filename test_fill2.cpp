//
// Created by serik1987 on 21.09.19.
//

#include "Application.h"
#include "data/Matrix.h"
#include "data/LocalMatrix.h"
#include "data/ContiguousMatrix.h"

void test_main(){
    using namespace std;

    logging::progress(0, 1, "Matrix initialization");

    mpi::Communicator& comm = Application::getInstance().getAppCommunicator();
    data::LocalMatrix A(comm, 21, 21, 1.0, 1.0);
    data::ContiguousMatrix B(comm, 21, 21, 1.0, 1.0);

    A.fill([](data::Matrix::Iterator& a){
        double x = a.getColumnUm();
        return 3*x;
    });

    logging::enter();
    logging::debug("Matrix A, after fill");
    A.printLocal();
    logging::debug("");
    logging::exit();

    B.fill([](data::Matrix::Iterator& a){
        double y = a.getRowUm();
        return 3*y;
    });

    logging::enter();
    logging::debug("Matrix B, after fill");
    B.printLocal();
    logging::debug("");
    logging::exit();

    B.synchronize();

    logging::enter();
    logging::debug("Matrix B, after synchronization");
    B.printLocal();
    logging::debug("");
    logging::exit();

    logging::progress(1, 1);
}