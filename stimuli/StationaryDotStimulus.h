//
// Created by serik1987 on 12.11.2019.
//

#ifndef MPI2_STATIONARYDOTSTIMULUS_H
#define MPI2_STATIONARYDOTSTIMULUS_H

#include "StationaryStimulus.h"
#include "DotStimulusParameters.h"

namespace stim {

    class StationaryDotStimulus : public StationaryStimulus, public DotStimulusParameters {

    protected:
        std::string getProcessorName() override { return "stim::StationaryDot"; }
        void loadStationaryStimulusParameters(const param::Object& source) override;
        void broadcastStationaryStimulusParameters() override;
        void setStationaryStimulusParameter(const std::string& name, const void* pvalue) override;
        void fillStimulusMatrix() override;

    public:
        explicit StationaryDotStimulus(mpi::Communicator& comm): StationaryStimulus(comm) {};

    };

}


#endif //MPI2_STATIONARYDOTSTIMULUS_H
