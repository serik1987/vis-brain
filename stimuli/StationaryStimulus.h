//
// Created by serik1987 on 11.11.2019.
//

#ifndef MPI2_STATIONARYSTIMULUS_H
#define MPI2_STATIONARYSTIMULUS_H

#include "Stimulus.h"
#include "../data/LocalMatrix.h"

namespace stim {

    class negative_prestimulus_epoch: public simulation_exception{
    public:
        const char* what() const noexcept override{
            return "Prestimulus epoch is negative";
        }
    };

    class negative_stimulus_duration: public simulation_exception{
    public:
        const char* what() const noexcept override{
            return "Stimulus duration is negative";
        }
    };

    class negative_poststimulus_epoch: public simulation_exception{
    public:
        const char* what() const noexcept override{
            return "Poststimulus epoch is negative";
        }
    };

    /**
     * Base class for all stationary stimuli
     * A stimulus is treated to be stationary if it doesn't depend on the input signal
     *
     * A stationary stimulus have two states: background and stimulus. At the background state
     * the background stimulus will be presented where each pixels eauals to the luminance At the stimulus state
     * stimulusMatrix is presented
     */
    class StationaryStimulus: public Stimulus {
    private:
        double prestimulusEpoch = 0.0;
        double stimulusDuration = 0.0;
        double poststimulusEpoch = 0.0;
        double time = 0.0;

    protected:
        data::LocalMatrix* stimulusMatrix = nullptr;

        void loadStimulusParameters(const param::Object& source) override;
        void broadcastStimulusParameters() override;
        void setStimulusParameter(const std::string& name, const void* pvalue) override;

        /**
         * Reads all stimulus parameters except 'type', 'mechanism', 'grid_x', 'grid_y',
         * 'size_x', 'size_y', 'luminance', 'contrast', 'prestimulus_epoch', 'stimulus_duration',
         * 'poststimulus_epoch'
         *
         * @param source
         */
        virtual void loadStationaryStimulusParameters(const param::Object& source) = 0;

        /**
         * Broadcasts all stimulus parameters except  'type', 'mechanism', 'grid_x', 'grid_y',
         * 'size_x', 'size_y', 'luminance', 'contrast', 'prestimulus_epoch', 'stimulus_duration',
         * 'poststimulus_epoch'
         */
        virtual void broadcastStationaryStimulusParameters() = 0;

        /**
         * Sets any stimulus parameter except  'type', 'mechanism', 'grid_x', 'grid_y',
         * 'size_x', 'size_y', 'luminance', 'contrast', 'prestimulus_epoch', 'stimulus_duration',
         * 'poststimulus_epoch'
         *
         * @param name name of this stimulus parameter
         * @param pvalue pointer to its value
         */
        virtual void setStationaryStimulusParameter(const std::string& name, const void* pvalue) = 0;

        /**
         * Performs al initialization routines except creating the background matrix
         */
        void initializeStimulus() override;

        /**
         * Destroys the stimulus.
         * Automatically calls during the object destruction
         * Shall be called after stimulus parameter has changed or when the simulation run has been completed
         * and you need to start another simulation run
         *
         * @param destruct false everywhere except in the destructor body
         */
        void finalizeProcessor(bool destruct = false) noexcept override;

        /**
         * Fills the stimulusMatrix - image values when the stimulus is present
         */
        virtual void fillStimulusMatrix() = 0;

    public:
        explicit StationaryStimulus(mpi::Communicator& comm): Stimulus(comm) {};

        ~StationaryStimulus() override{
            finalizeProcessor(true);
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
        double getStimulusStart(){
            return prestimulusEpoch;
        }

        /**
         *
         * @return time of stimulus finish
         */
        double getStimulusFinish(){
            return prestimulusEpoch + stimulusDuration;
        }

        double getRecordLength() override{
            return prestimulusEpoch + stimulusDuration + poststimulusEpoch;
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

        [[nodiscard]] data::Matrix& getOutput() override;

        /**
         * Runs at each iteration
         *
         * @param t current timestamp in milliseconds
         */
        void update(double t) override { time = t; }

        /**
         * Creates new stationary stimulus with a given mechanism
         *
         * @param mechanism the mechanism to create
         * @param comm communicator
         * @return pointer to the stimulus
         */
        static StationaryStimulus* createStationaryStimulus(mpi::Communicator& comm, const std::string& mechanism);
    };

}


#endif //MPI2_STATIONARYSTIMULUS_H
