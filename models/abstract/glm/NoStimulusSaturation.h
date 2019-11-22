//
// Created by serik1987 on 18.11.2019.
//

#ifndef MPI2_NOSTIMULUSSATURATION_H
#define MPI2_NOSTIMULUSSATURATION_H

#include "StimulusSaturation.h"

namespace equ {

    /**
     * Represents no saturation, just a single stimulus amplification
     */
    class NoStimulusSaturation: public StimulusSaturation {
    protected:
        [[nodiscard]] std::string getProcessorName() override { return "equ::NoStimulusSaturation"; }
        void loadSaturationParameterList(const param::Object& source) override {};
        void broadcastSaturationParameterList() override {};
        void setSaturationParameter(const std::string& name, const void* pvalue) override;
        void initializeStimulusSaturation() override {};
        double getSaturationOutput(double saturationInput) override { return saturationInput; }

    public:
        explicit NoStimulusSaturation(mpi::Communicator& comm): StimulusSaturation(comm), Equation(comm),
            Processor(comm) {};
    };

}


#endif //MPI2_NOSTIMULUSSATURATION_H
