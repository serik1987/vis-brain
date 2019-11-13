//
// Created by serik1987 on 13.11.2019.
//

#ifndef MPI2_MOVINGRECTANGULARGRATING_H
#define MPI2_MOVINGRECTANGULARGRATING_H

#include "MovingGrating.h"


namespace stim {

    class MovingRectangularGrating: public MovingGrating {
    protected:
        [[nodiscard]] std::string getProcessorName() override { return "stim:MovingRectangularGrating"; }
        void updateMovingStimulus(double relativeTime) override;

    public:
        explicit MovingRectangularGrating(mpi::Communicator& comm): MovingGrating(comm) {};
    };

}


#endif //MPI2_MOVINGRECTANGULARGRATING_H
