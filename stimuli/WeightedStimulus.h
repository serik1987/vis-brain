//
// Created by serik1987 on 16.11.2019.
//

#ifndef MPI2_WEIGHTEDSTIMULUS_H
#define MPI2_WEIGHTEDSTIMULUS_H

#include "ComplexStimulus.h"

namespace stim {

    /**
     * Weighted stimulus. Each pixel in the weighted stimulus is defined as:
     * L =  w1 * l1 + w2 * l2 + w3 * l3 where
     * l1, l2, l3 - weights of the constituent stimuli
     * w1, w2, w3 - some coefficients derived from the 'weights' property
     */
    class WeightedStimulus: public ComplexStimulus {
    private:
        std::vector<double> weights;

    protected:
        [[nodiscard]] std::string getProcessorName() override { return "stim:WeightedStimulus"; }
        void loadComplexStimulusParameters(const param::Object& source) override;
        void broadcastComplexStimulusParameters() override;
        void setComplexStimulusParameter(const std::string& name, const void* pvalue) override;
        void initializeComplexStimulus() override;
        void finalizeComplexStimulus(bool destruct) override;

    public:
        explicit WeightedStimulus(mpi::Communicator& comm): ComplexStimulus(comm) {};

        ~WeightedStimulus() override {
            finalizeComplexStimulus(true);
        }

        class weight_vector_inconsistency: public simulation_exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "Number of weights in the 'weight' property is not the same"
                       "as number of child stimuli in 'content' property";
            }
        };

        void update(double time) override;
    };

}


#endif //MPI2_WEIGHTEDSTIMULUS_H
