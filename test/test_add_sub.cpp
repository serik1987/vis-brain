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
    logging::debug("Matrix -A");
    (-A).printLocal();
    logging::debug("");
    logging::debug("Matrix -B");
    (-B).printLocal();
    logging::debug("");
    logging::debug("Matrix ++A");
    (++A).printLocal();
    logging::debug("");
    logging::debug("after increment: ");
    A.printLocal();
    logging::debug("");
    logging::debug("Matrix ++B");
    (++B).printLocal();
    logging::debug("");
    logging::debug("after increment: ");
    B.printLocal();
    logging::debug("");
    logging::debug("Matrix --A");
    (--A).printLocal();
    logging::debug("");
    logging::debug("after decrement:");
    A.printLocal();
    logging::debug("");
    logging::debug("Matrix --B");
    (--B).printLocal();
    logging::debug("");
    logging::exit();

    logging::enter();
    logging::debug("Test of A+=B");
    (A+=B).printLocal();
    logging::debug("");
    logging::debug("Matrix A, after increment");
    A.printLocal();
    logging::debug("");
    logging::exit();

    logging::enter();
    logging::debug("Test of A-=B");
    (A-=B).printLocal();
    logging::debug("");
    logging::debug("Matrix A, after decrement");
    A.printLocal();
    logging::debug("");
    logging::exit();

    logging::enter();
    logging::debug("Text of A+=10");
    (A+=10).printLocal();
    logging::debug("");
    logging::debug("Matrix A, after increment");
    A.printLocal();
    logging::debug("");
    logging::exit();

    logging::enter();
    logging::debug("Test of A-=10");
    (A-=10).printLocal();
    logging::debug("");
    logging::debug("Matrix B, after decrement");
    A.printLocal();
    logging::debug("");
    logging::exit();

    logging::enter();
    logging::debug("Test of C.add(A, B)");
    C.add(A, B).printLocal();
    logging::debug("Matrix C, after addition");
    C.printLocal();
    logging::debug("");
    logging::exit();

    logging::enter();
    logging::debug("Test of C.sub(A, B)");
    C.sub(A, B).printLocal();
    logging::debug("");
    logging::debug("Matrix C, after substraction");
    C.printLocal();
    logging::debug("");
    logging::exit();

    logging::enter();
    logging::debug("Test of C.add(A, 10)");
    C.add(A, 10).printLocal();
    logging::debug("");
    logging::debug("Matrix C, after addition");
    C.printLocal();
    logging::debug("");
    logging::exit();

    logging::enter();
    logging::debug("Test of C.sub(A, 10)");
    C.sub(A, 10).printLocal();
    logging::debug("");
    logging::debug("Matrix C, after substraction");
    C.printLocal();
    logging::debug("");
    logging::exit();

    logging::enter();
    logging::debug("Test of C.sub(10, A)");
    C.sub(10, A).printLocal();
    logging::debug("");
    logging::debug("Matrix C, after substraction");
    C.printLocal();
    logging::debug("");
    logging::exit();

    logging::enter();
    logging::debug("Test of C.neg(A)");
    C.neg(A).printLocal();
    logging::debug("");
    logging::debug("Matrix C, after negoiation");
    C.printLocal();
    logging::debug("");
    logging::exit();


    logging::progress(1, 1);
}