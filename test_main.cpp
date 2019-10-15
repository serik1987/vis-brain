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

    data::ContiguousMatrix source(comm, 2100, 2100, 1.0, 1.0);
    data::LocalMatrix result(comm, 210, 210, 1.0, 1.0);
    data::reader::BinReader reader("source.bin");

    const double sigma = 0.2;

    for (auto a = source.begin(); a != source.end(); ++a){
        double x = a.getColumnUm();
        double y = a.getRowUm();
        *a = exp(-(x*x + y*y)/(2 * sigma * sigma));
    }
    double sum = source.sum();
    for (auto a = source.begin(); a != source.end(); ++a){
        *a /= sum;
    }
    source.synchronize();
    reader.save(source);

    logging::progress(0, 1, "Downsampling");
    result.downsample(source);
    reader.save(result, "target.bin");

    logging::progress(1, 1);

}