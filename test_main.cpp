//
// Created by serik1987 on 21.09.19.
//


#include "data/ContiguousMatrix.h"
#include "data/reader/BinReader.h"

void test_main(){
    using namespace std;

    logging::progress(0, 1, "Matrix initialization");

    mpi::Communicator& comm = Application::getInstance().getAppCommunicator();

    const int field_width = 256;
    const double field_width_deg = 1.0;
    const double res_x = field_width_deg / field_width;
    const int kernel_width = 21;
    const double kernel_width_deg = kernel_width * res_x;

    const int field_height = field_width;
    const double field_height_deg = field_width_deg;
    const int kernel_height = kernel_width;
    const double kernel_height_deg = kernel_width_deg;

    const double sigma = 0.015;

    data::ContiguousMatrix A(comm, field_width, field_height, field_width_deg, field_height_deg);
    A.fill([](auto a){
        double x = a.getColumnUm();
        double y = a.getRowUm();
        if (abs(x) <= 0.25 && abs(y) <= 0.25){
            return 1.0;
        } else {
            return 0.0;
        }
    });
    A.synchronize();

    data::ContiguousMatrix K(comm, kernel_width, kernel_height, kernel_width_deg, kernel_height_deg);
    K.fill([sigma](auto a){
        double x = a.getColumnUm();
        double y = a.getRowUm();
        double value = exp(-(x*x + y*y)/(2*sigma*sigma));
        return value;
    });
    double S = K.sum();
    K.fill([S](auto a){
        return *a/S;
    });
    K.synchronize();

    logging::progress(0, 1, "Saving initial matrices");
    data::reader::BinReader reader("matrixA.bin");
    reader.save(A);
    reader.save(K, "matrixK.bin");

    logging::progress(0, 1, "Providing convolution");
    data::LocalMatrix B(comm, field_width, field_height, field_width_deg, field_height_deg);
    B.convolve(K, A);
    reader.save(B, "matrixB.bin");

    logging::progress(1, 1);
}