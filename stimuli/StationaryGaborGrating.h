//
// Created by serik1987 on 12.11.2019.
//

#ifndef MPI2_STATIONARYGABORGRATING_H
#define MPI2_STATIONARYGABORGRATING_H

#include "StationaryGrating.h"

namespace stim {

    class StationaryGaborGrating : public StationaryGrating {
    protected:
        std::string getProcessorName() override { return "stim::StationaryGaborGrating"; }
        void fillStimulusMatrix() override;

    public:
        explicit StationaryGaborGrating(mpi::Communicator& comm): StationaryGrating(comm) {};
    };

}


#endif //MPI2_STATIONARYGABORGRATING_H
