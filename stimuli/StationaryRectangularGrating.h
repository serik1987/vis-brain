//
// Created by serik1987 on 12.11.2019.
//

#ifndef MPI2_STATIONARYRECTANGULARGRATING_H
#define MPI2_STATIONARYRECTANGULARGRATING_H

#include "StationaryGrating.h"

namespace stim {

    class StationaryRectangularGrating: public StationaryGrating {
    protected:
        std::string getProcessorName() override { return "stim::StationaryRectangularGrating"; }
        void fillStimulusMatrix() override;

    public:
        explicit StationaryRectangularGrating(mpi::Communicator& comm): StationaryGrating(comm) {};
    };

}


#endif //MPI2_STATIONARYRECTANGULARGRATING_H
