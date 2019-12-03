//
// Created by serik1987 on 01.12.2019.
//

#include "VsdAnalyzer.h"
#include "../log/output.h"

namespace analysis{

    void VsdAnalyzer::initialize() {
        mpi::Communicator& comm = getInputCommunicator();
        int width = getMatrixWidth();
        int height = getMatrixHeight();
        double widthUm = getMatrixWidthUm();
        double heightUm = getMatrixHeightUm();
        output = new data::LocalMatrix(comm, width, height, widthUm, heightUm, 0.0);

        initializeVsd();
    }
}