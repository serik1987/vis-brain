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

    A.fill([](data::Matrix::Iterator a){
        return a.getIndex();
    });

    logging::enter();
    logging::debug("Matrix A, initialized");
    A.printLocal();
    logging::debug("");
    logging::exit();

    logging::progress(0, 1, "Type I reduction");

    double s = A.reduce(0, MPI_SUM, [](double& x, double y){
        x += y;
    });

    logging::enter();
    logging::debug("Tyoe I reduction: " + to_string(s));
    logging::exit();

    logging::progress(1, 1);
}