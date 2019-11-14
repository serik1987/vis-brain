//
// Created by serik1987 on 13.11.2019.
//

#ifndef MPI2_MOVINGGRATING_H
#define MPI2_MOVINGGRATING_H

#include "MovingStimulus.h"
#include "GratingStimulusParameters.h"

namespace stim {

    class MovingGrating: public MovingStimulus, public GratingStimulusParameters {
    private:
        double temporalFrequency = 0.0;

        void loadMovingStimulusParameters(const param::Object& source) override;
        void broadcastMovingStimulusParameters() override;
        void setMovingStimulusParameter(const std::string& name, const void* pvalue) override;

    public:
        explicit MovingGrating(mpi::Communicator& comm): MovingStimulus(comm), GratingStimulusParameters() {};

        class negative_temporal_frequency: public simulation_exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "Attempt to set negative or zero temporal frequency to the grating stimulus";
            }
        };

        /**
         *
         * @return temporal frequency of the moving grating in Hz
         */
        [[nodiscard]] double getTemporalFrequency() const { return temporalFrequency; }

        void setTemporalFrequency(double value){
            if (value > 0){
                temporalFrequency = value;
            } else {
                throw negative_temporal_frequency();
            }
        }
    };

}


#endif //MPI2_MOVINGGRATING_H
