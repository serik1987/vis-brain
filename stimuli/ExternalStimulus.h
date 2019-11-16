//
// Created by serik1987 on 14.11.2019.
//

#ifndef MPI2_EXTERNALSTIMULUS_H
#define MPI2_EXTERNALSTIMULUS_H

#include "MovingStimulus.h"
#include "../sys/ExternalMechanism.h"

namespace stim {

    /**
     * A base class for all stimuli which implementations are separated from the main executable file
     * (These stimuli are presented in .so files which are placed to the folder 'stimuli'
     */
    class ExternalStimulus: public MovingStimulus, public sys::ExternalMechanism {
    private:
        int (*get_stimulus_value)(double x, double y, double t, double* pvalue);

    protected:
        [[nodiscard]] std::string getProcessorName() override{ return "stim:external"; }
        void loadMovingStimulusParameters(const param::Object& source) override;
        void broadcastMovingStimulusParameters() override;
        void setMovingStimulusParameter(const std::string& name, const void* pvalue) override;
        void initializeExtraBuffer() override;
        void updateMovingStimulus(double relativeTime) override;
        void finalizeExtraBuffer(bool destruct = false) override;
    public:
        /**
         *
         * @param comm communicator responsible for the stimulus
         * @param filename absolute path to the mechanism (.so file)
         */
        ExternalStimulus(mpi::Communicator& comm, const std::string& filename):
            MovingStimulus(comm), ExternalMechanism(filename) {
            get_stimulus_value = (int (*)(double, double, double, double*))loadFunction("get_stimulus_value");
        };

        ~ExternalStimulus() override{
            finalizeExtraBuffer(true);
        }

        class get_stimulus_value_error: public simulation_exception{
        public:
            const char* what() const noexcept override{
                return "Error when calculating pixel value";
            }
        };
    };

}


#endif //MPI2_EXTERNALSTIMULUS_H
