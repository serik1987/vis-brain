//
// Created by serik1987 on 18.11.2019.
//

#ifndef MPI2_HALFSIGMOIDSTIMULUSSATURATION_H
#define MPI2_HALFSIGMOIDSTIMULUSSATURATION_H

#include "StimulusSaturation.h"

namespace equ {

    class HalfSigmoidStimulusSaturation: public StimulusSaturation {
    protected:
        [[nodiscard]] std::string getProcessorName() override { return "equ::HalfSigmoidStimulusSaturation"; }
        void loadSaturationParameterList(const param::Object& source) override;
        void broadcastSaturationParameterList() override;
        void setSaturationParameter(const std::string& name, const void* pvalue) override;
        void initializeStimulusSaturation() override {};
        double getSaturationOutput(double saturationInput) override;

    public:
        explicit HalfSigmoidStimulusSaturation(mpi::Communicator& comm): StimulusSaturation(comm),
            Equation(comm), Processor(comm) {};
    };

}


#endif //MPI2_HALFSIGMOIDSTIMULUSSATURATION_H
