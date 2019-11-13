//
// Created by serik1987 on 12.11.2019.
//

#ifndef MPI2_STATIONARYBARSTIMULUS_H
#define MPI2_STATIONARYBARSTIMULUS_H

#include "StationaryStimulus.h"
#include "BarStimulusParameters.h"

namespace stim {

    class StationaryBarStimulus: public StationaryStimulus, public BarStimulusParameters {

    protected:
        [[nodiscard]] std::string getProcessorName() override { return "stim::StationaryBarStimulus"; }
        void loadStationaryStimulusParameters(const param::Object& source) override;
        void broadcastStationaryStimulusParameters() override;
        void setStationaryStimulusParameter(const std::string& name, const void* pvalue) override;
        void fillStimulusMatrix() override;

    public:
        explicit StationaryBarStimulus(mpi::Communicator& comm): StationaryStimulus(comm) {};
    };

}


#endif //MPI2_STATIONARYBARSTIMULUS_H
