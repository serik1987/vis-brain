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
    data::LocalMatrix C(comm, 21, 21, 1.0, 1,0);

    A.fill([](data::Matrix::Iterator a){
        return a.getRowUm() * 21;
    });

    B.fill([](data::Matrix::Iterator b){
        return b.getColumnUm() * 21;
    });

    logging::enter();
    logging::debug("Matrix A, after initialization");
    A.printLocal();
    logging::debug("");
    logging::debug("Matrix B, after initialization");
    B.printLocal();
    logging::debug("");
    logging::exit();



    logging::progress(1, 1);
}