//
// Created by serik1987 on 13.11.2019.
//

#ifndef MPI2_STEPSTIMULUSPARAMETERS_H
#define MPI2_STEPSTIMULUSPARAMETERS_H

#include "../log/exceptions.h"
#include "../param/Object.h"

namespace stim {

    /**
     * A base class for all stimuli that contains step stimulus parameters. The following parameters are treated as
     * step stimulus parameters:
     * prestimulus_epoch, stimulus_duration, poststimulus_epoch
     */
    class StepStimulusParameters {
    private:
        double prestimulusEpoch = 0.0;
        double stimulusDuration = 0.0;
        double poststimulusEpoch = 0.0;

    protected:
        void loadStepStimulusParameters(const param::Object& source);
        void broadcastStepStimulusParameter();
        void setStepStimulusParameter(const std::string& name, const void* pvalue);

    public:

        class negative_prestimulus_epoch: public simulation_exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "Prestimulus epoch is negative";
            }
        };

        class negative_stimulus_duration: public simulation_exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "Stimulus duration is negative";
            }
        };

        class negative_poststimulus_epoch: public simulation_exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "Poststimulus epoch is negative";
            }
        };

        /**
         *
         * @return prestimulus epoch in ms
         */
        [[nodiscard]] double getPrestimulusEpoch() const { return prestimulusEpoch; }

        /**
         *
         * @return stimulus duration in ms
         */
        [[nodiscard]] double getStimulusDuration() const { return stimulusDuration; }

        /**
         *
         * @return poststimulus epoch in ms
         */
        [[nodiscard]] double getPoststimulusEpoch() const { return poststimulusEpoch; }

        /**
         *
         * @return time of stimulus start
         */
        [[nodiscard]] double getStimulusStart() const{
            return prestimulusEpoch;
        }

        /**
         *
         * @return time of stimulus finish
         */
        [[nodiscard]] double getStimulusFinish() const{
            return prestimulusEpoch + stimulusDuration;
        }

        /**
         * Sets the time from the beginning of the record to the stimulus onset
         *
         * @param value prestimulus epoch in ms
         */
        void setPrestimulusEpoch(double value){
            if (value < 0.0){
                throw negative_prestimulus_epoch();
            }
            prestimulusEpoch = value;
        }

        /**
         * Sets the stimulus duration
         *
         * @param value stimulus duration in ms
         */
        void setStimulusDuration(double value){
            if (value < 0.0){
                throw negative_stimulus_duration();
            }
            stimulusDuration = value;
        }

        /**
         * Sets the time from the stimulus offset to the end of the record
         *
         * @param value poststimulus time in ms
         */
        void setPoststimulusEpoch(double value){
            if (value < 0.0){
                throw negative_poststimulus_epoch();
            }
            poststimulusEpoch = value;
        }

        double getRecordLength() const{
            return prestimulusEpoch + stimulusDuration + poststimulusEpoch;
        }

    };

}


#endif //MPI2_STEPSTIMULUSPARAMETERS_H
