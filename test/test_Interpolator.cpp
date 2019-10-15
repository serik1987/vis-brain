//
// Created by serik1987 on 21.09.19.
//


#include "data/ContiguousMatrix.h"
#include "data/reader/BinReader.h"
#include "data/Interpolator.h"

void test_main(){
    using namespace std;

    logging::progress(0, 1, "Matrix initialization");

    mpi::Communicator& comm = Application::getInstance().getAppCommunicator();

    data::ContiguousMatrix A(comm, 21, 21, 1.0, 1.0);
    data::ContiguousMatrix Asmooth(comm, 210, 210, 1.0, 1.0);
    data::reader::BinReader reader("matrixA.bin");

    const double sigma = 0.2;

    for (auto a = A.begin(); a != A.end(); ++a){
        double x = a.getColumnUm();
        double y = a.getRowUm();
        *a = exp(-(x*x + y*y)/(2 * sigma * sigma));
    }
    double sum = A.sum();
    for (auto a = A.begin(); a != A.end(); ++a){
        *a /= sum;
    }
    A.synchronize();

    logging::progress(0, 1, "Interpolation");
    data::Interpolator interpolator(Asmooth, A);
    interpolator.interpolate(Asmooth, A);
    reader.save(Asmooth);

    logging::progress(1, 1);
}