//
// Created by serik1987 on 21.09.19.
//

#include "data/LocalMatrix.h"
#include "data/noise/UniformNoiseGenerator.h"
#include "data/stream/BinStream.h"



void test_main(){
    using namespace std;

    logging::progress(0, 1, "Noise generation");

    mpi::Communicator& comm = Application::getInstance().getAppCommunicator();
    data::LocalMatrix matrix(comm, 20, 20, 1.0, 1.0);
    data::noise::UniformNoiseGenerator gen(-1.0, 1.0);
    data::stream::BinStream stream(&matrix, "output.bin", data::stream::Stream::Write, 100.0);

    for (int i=0; i < 10000; ++i){
        gen.fill(matrix);
        stream.write();
    }


    logging::progress(1, 1);

}