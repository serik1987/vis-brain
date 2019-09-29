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

    logging::enter();
    logging::debug("A*=B test");
    (A*=B).printLocal();
    logging::debug("");
    logging::debug("Matrix A, after multiplication");
    A.printLocal();
    logging::debug("");
    logging::exit();

    logging::enter();
    logging::debug("A/=B test");
    (A/=B).printLocal();
    logging::debug("");
    logging::debug("Matrix A, after division");
    A.printLocal();
    logging::debug("");
    logging::exit();

    logging::enter();
    logging::debug("A*=2 test");
    (A*=2).printLocal();
    logging::debug("");
    logging::debug("Matrix A, after multiplication");
    A.printLocal();
    logging::debug("");
    logging::exit();

    logging::enter();
    logging::debug("A/=2 test");
    (A/=2).printLocal();
    logging::debug("");
    logging::debug("Matrix A, after division");
    A.printLocal();
    logging::debug("");
    logging::exit();

    logging::enter();
    logging::debug("mul(...) test");
    C.mul(A, B).printLocal();
    logging::debug("");
    logging::debug("Matrix C, after multiplication");
    C.printLocal();
    logging::debug("");
    logging::exit();

    logging::enter();
    logging::debug("div(...) test");
    C.div(A, B).printLocal();
    logging::debug("");
    logging::debug("Matrix C, after division");
    C.printLocal();
    logging::debug("");
    logging::exit();

    logging::enter();
    logging::debug("mul(...) test");
    C.mul(B, 2).printLocal();
    logging::debug("");
    logging::debug("Matrix C, after multiplication");
    C.printLocal();
    logging::debug("");
    logging::exit();

    logging::enter();
    logging::debug("div(...) test");
    C.div(B, 2).printLocal();
    logging::debug("");
    logging::debug("M<atrix C, after division");
    C.printLocal();
    logging::debug("");
    logging::exit();

    logging::enter();
    logging::debug("mul(...) test");
    C.mul(2, A).printLocal();
    logging::debug("");
    logging::debug("Matrix C, after multiplication");
    C.printLocal();
    logging::debug("");
    logging::exit();

    logging::enter();
    logging::debug("div(...) test");
    C.div(1, B).printLocal();
    logging::debug("");
    logging::debug("Matrix C, after division");
    C.printLocal();
    logging::debug("");
    logging::exit();

    logging::progress(1, 1);
}