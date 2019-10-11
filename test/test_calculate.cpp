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
    data::LocalMatrix A(comm, 21, 21, 1.0, 1.0);
    data::ContiguousMatrix B(comm, 21, 21, 1.0, 1.0);

    A.fill([](data::Matrix::Iterator a){
        return 3 * a.getColumnUm();
    });
    B.fill([](data::Matrix::Iterator a){
        return 2 * a.getRowUm();
    });
    B.synchronize();

    logging::enter();
    logging::debug("A, after initialization");
    A.printLocal();
    logging::debug("");
    logging::debug("B, after initialization");
    B.printLocal();
    logging::debug("");
    logging::exit();

    logging::progress(0, 1, "one-side filling");

    A.calculate(B, [](data::Matrix::ConstantIterator b){
        return 0.28 * *b;
    });

    logging::enter();
    logging::debug("A, after calculation");
    A.printLocal();
    logging::debug("");
    logging::exit();

    logging::progress(0, 1, "two-side filling");

    data::LocalMatrix C(comm, 21, 21, 1.0, 1.0);
    C.calculate(A, B, [](data::Matrix::ConstantIterator& b, data::Matrix::ConstantIterator& c){
        return *b + *c;
    });

    logging::enter();
    logging::debug("Matrix C");
    C.printLocal();
    logging::debug("");
    logging::exit();

    logging::progress(1, 1);
}