//
// Created by serik1987 on 11.11.2019.
//

#ifndef MPI2_STATIONARYGRATING_H
#define MPI2_STATIONARYGRATING_H

#include "StationaryStimulus.h"
#include "GratingStimulusParameters.h"

namespace stim {

    /**
     * Represents a single stationary grating
     */
    class StationaryGrating: public StationaryStimulus, public GratingStimulusParameters {

    protected:
        void loadStationaryStimulusParameters(const param::Object& source) override;
        void broadcastStationaryStimulusParameters() override;
        void setStationaryStimulusParameter(const std::string& name, const void* pvalue) override;

    public:
        explicit StationaryGrating(mpi::Communicator& comm): StationaryStimulus(comm) {};
    };

}


#endif //MPI2_STATIONARYGRATING_H
