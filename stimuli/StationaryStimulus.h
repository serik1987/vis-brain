//
// Created by serik1987 on 11.11.2019.
//

#ifndef MPI2_STATIONARYSTIMULUS_H
#define MPI2_STATIONARYSTIMULUS_H

#include "Stimulus.h"
#include "StepStimulusParameters.h"
#include "../data/LocalMatrix.h"

namespace stim {

    /**
     * Base class for all stationary stimuli
     * A stimulus is treated to be stationary if it doesn't depend on the input signal
     *
     * A stationary stimulus have two states: background and stimulus. At the background state
     * the background stimulus will be presented where each pixels eauals to the luminance At the stimulus state
     * stimulusMatrix is presented
     */
    class StationaryStimulus: public Stimulus, public StepStimulusParameters {
    private:
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

        /**
         *
         * @return record length for the stimulus
         */
        [[nodiscard]] double getRecordLength() const override{
            return StepStimulusParameters::getRecordLength();
        }
    };

}


#endif //MPI2_STATIONARYSTIMULUS_H
