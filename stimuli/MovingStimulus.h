//
// Created by serik1987 on 13.11.2019.
//

#ifndef MPI2_MOVINGSTIMULUS_H
#define MPI2_MOVINGSTIMULUS_H

#include "Stimulus.h"
#include "StepStimulusParameters.h"
#include "../data/LocalMatrix.h"

namespace stim {

    class MovingStimulus: public Stimulus, public StepStimulusParameters {
    protected:
        data::LocalMatrix* meanLuminance = nullptr;
        bool showStimulus = false;

        void initializeStimulus() override;
        void finalizeProcessor(bool destruct = false) noexcept override;
        void loadStimulusParameters(const param::Object& source) override;
        void broadcastStimulusParameters() override;
        void setStimulusParameter(const std::string& name, const void* pvalue) override;

        /**
         * Loads all parameters of the moving stimulus except luminance, contrast,
         * prestimulus_epoch, stimulus_duration, poststimulus_epoch
         *
         * @param source parameter list
         */
        virtual void loadMovingStimulusParameters(const param::Object& source) = 0;

        /**
         * Broadcasts all stimulus parameters except luminance, contrast, prestimulus_epoch, stimulus_duration,
         * poststimulus_epoch
         */
        virtual void broadcastMovingStimulusParameters() = 0;

        /**
         * Sets any stimulus parameter except luminance, contrast, prestimulus_epoch, stimulus_duration,
         * poststimulus_epoch
         *
         * @param name name of the stimulus parameter
         * @param pvalue pointer to its value
         */
        virtual void setMovingStimulusParameter(const std::string& name, const void* pvalue) = 0;

        /**
         * Updates the moving stimulus when the moving stimulus itself is presented
         *
         * @param relativeTime time from the stimulus onset
         */
        virtual void updateMovingStimulus(double relativeTime) = 0;

    public:
        explicit MovingStimulus(mpi::Communicator& comm): Stimulus(comm), StepStimulusParameters() {};

        ~MovingStimulus(){
            finalizeProcessor(true);
        }

        /**
         * Creates a pointer to the certain child instance
         *
         * @param comm communicator for which an instance shall be created
         * @param mechanism minor field of the mechanism name
         * @return pointer to the MovingStimulus instance
         */
        static MovingStimulus* createMovingStimulus(mpi::Communicator& comm, const std::string& mechanism);

        /**
         *
         * @return total length of the record
         */
        double getRecordLength() override {
            return StepStimulusParameters::getRecordLength();
        }

        /**
         * Updates the stimulus
         *
         * @param time stimulus time from the beginning of the experiment
         */
        void update(double time) override;

        /**
         *
         * @return the stimulus output
         */
        data::Matrix& getOutput() override{
            if (showStimulus){
                return *output;
            } else {
                return *meanLuminance;
            }
        }
    };

}


#endif //MPI2_MOVINGSTIMULUS_H
