//
// Created by serik1987 on 13.11.2019.
//

#ifndef MPI2_MOVINGGABORGRATING_H
#define MPI2_MOVINGGABORGRATING_H

#include "MovingGrating.h"

namespace stim {

    class MovingGaborGrating: public MovingGrating {
    protected:
        std::string getProcessorName() override { return "stim::MovingGaborGrating"; }
        void updateMovingStimulus(double relativeTime) override;

    public:
        explicit MovingGaborGrating(mpi::Communicator& comm): MovingGrating(comm) {};
    };

}


#endif //MPI2_MOVINGGABORGRATING_H
