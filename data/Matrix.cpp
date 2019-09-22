//
// Created by serik1987 on 21.09.19.
//

#include "Matrix.h"
#include "../Application.h"

namespace data{

    Matrix::Matrix(mpi::Communicator &comm, int w, int h, double w_um, double h_um, double filler): communicator(comm),
    width(w), height(h), widthUm(w_um), heightUm(h_um)
    {
        size = width * height;
        rank = comm.getRank();
        localSize = ceil((double)size/comm.getProcessorNumber());
        iStart = localSize * rank;
        iFinish = iStart + localSize;
        if (iFinish > size){
            iFinish = size;
            localSize = iFinish - iStart;
        }
    }

    void Matrix::printLocal(){
        using namespace std;

        for (int i=0; i < height; i++){
            ostringstream ss;
            for (int j=0; j < width; j++){
                try {
                    ss << getValue(i, j) << "\t";
                } catch (std::exception& e){
                    ss << "XXXXX\t";
                }
            }
            Application::getInstance().getLoggingEngine().debug(ss.str());
        }
        Application::getInstance().getLoggingEngine().debug("");
    }

}