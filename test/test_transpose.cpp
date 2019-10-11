//
// Created by serik1987 on 21.09.19.
//

#include "../Application.h"
#include "../data/Matrix.h"
#include "../data/LocalMatrix.h"
#include "../data/ContiguousMatrix.h"

void test_main(){
    using namespace std;

    logging::progress(0, 1, "Matrix initialization");

    mpi::Communicator& comm = Application::getInstance().getAppCommunicator();
    data::ContiguousMatrix A(comm, 6, 6, 1.0, 1.0);
    data::ContiguousMatrix B(comm, 6, 6, 1.0, 1.0);
    data::LocalMatrix C(comm, 6, 6, 1.0, 1,0);

    A.getValue(0, 0) = 35;
    A.getValue(0, 1) = 1;
    A.getValue(0, 2) = 6;
    A.getValue(0, 3) = 26;
    A.getValue(0, 4) = 19;
    A.getValue(0, 5) = 24;

    A.getValue(1, 0) = 3;
    A.getValue(1, 1) = 32;
    A.getValue(1, 2) = 7;
    A.getValue(1, 3) = 21;
    A.getValue(1, 4) = 23;
    A.getValue(1, 5) = 25;

    A.getValue(2, 0) = 31;
    A.getValue(2, 1) = 9;
    A.getValue(2, 2) = 2;
    A.getValue(2, 3) = 22;
    A.getValue(2, 4) = 27;
    A.getValue(2, 5) = 20;

    A.getValue(3, 0) = 8;
    A.getValue(3, 1) = 28;
    A.getValue(3, 2) = 33;
    A.getValue(3, 3) = 17;
    A.getValue(3, 4) = 10;
    A.getValue(3, 5) = 15;

    A.getValue(4, 0) = 30;
    A.getValue(4, 1) = 5;
    A.getValue(4, 2) = 34;
    A.getValue(4, 3) = 12;
    A.getValue(4, 4) = 14;
    A.getValue(4, 5) = 16;

    A.getValue(5, 0) = 4;
    A.getValue(5, 1) = 36;
    A.getValue(5, 2) = 29;
    A.getValue(5, 3) = 13;
    A.getValue(5, 4) = 18;
    A.getValue(5, 5) = 11;

    logging::enter();
    logging::debug("Matrix A, after initialization");
    A.printLocal();
    logging::debug("");
    logging::exit();

    logging::enter();
    logging::debug("Matrix transpose test");
    A.transpose().printLocal();
    logging::debug("");
    logging::debug("After transpose");
    A.printLocal();
    logging::debug("");
    logging::debug("After synchronize");
    logging::exit();
    A.synchronize();
    logging::enter();
    A.printLocal();
    logging::debug("");
    logging::exit();

    logging::enter();
    logging::debug("Matrix transpose test # 2");
    B.transpose(A).printLocal();
    logging::debug("");
    logging::debug("After transpose:");
    B.printLocal();
    logging::debug("");
    logging::exit();

    B.synchronize();
    logging::enter();
    logging::debug("Matrix B, after synchronize");
    B.printLocal();
    logging::debug("");
    logging::exit();



    logging::progress(1, 1);
}