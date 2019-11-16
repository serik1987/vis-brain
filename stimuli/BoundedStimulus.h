//
// Created by serik1987 on 16.11.2019.
//

#ifndef MPI2_BOUNDEDSTIMULUS_H
#define MPI2_BOUNDEDSTIMULUS_H

#include "ComplexStimulus.h"

namespace stim {

    class BoundedStimulus: public ComplexStimulus {
    protected:
        [[nodiscard]] std::string getProcessorName() override { return "stim::BoundedStimulus"; }
        void loadComplexStimulusParameters(const param::Object& source) override;
        void broadcastComplexStimulusParameters() override;
        void setComplexStimulusParameter(const std::string& name, const void* pvalue) override;
        void initializeComplexStimulus() override;
        void update(double time) override;
        void finalizeComplexStimulus(bool destruct) override;

    public:
        explicit BoundedStimulus(mpi::Communicator& comm): ComplexStimulus(comm) {};

        ~BoundedStimulus() override {
            finalizeComplexStimulus(true);
        }
    };

}


#endif //MPI2_BOUNDEDSTIMULUS_H
