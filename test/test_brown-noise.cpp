//
// Created by serik1987 on 21.09.19.
//

#include "data/noise/distributions.h"
#include "data/noise/noise.h"
#include "data/LocalMatrix.h"
#include "data/stream/BinStream.h"

void test_main(){
    using namespace std;

    logging::progress(0, 1, "Test of random number engine");

    mpi::Communicator& comm = Application::getInstance().getAppCommunicator();
    data::noise::NormalDistribution dist;
    data::LocalMatrix output(comm, 50, 50, 1.0, 1.0);
    data::noise::BrownNoise noise(output, dist, 0.1);
    data::stream::BinStream stream(&output, "out.bin", data::stream::Stream::Write, 10.0);

    for (int i=0; i < 1000; i++){
        noise.update();
        stream.write();
    }

    logging::progress(1, 1);

}