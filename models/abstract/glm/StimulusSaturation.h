//
// Created by serik1987 on 18.11.2019.
//

#ifndef MPI2_STIMULUSSATURATION_H
#define MPI2_STIMULUSSATURATION_H

#include "../../../processors/Equation.h"

namespace equ {

    /**
     * A base class for all stimulus saturation mechanisms
     * Any visual stimulus that is introduced into GLM model shall be passed through the saturation
     * mechanism
     */
    class StimulusSaturation: virtual public Equation {
    private:
        double darkCurrent = -1.0;
        double stimulusAmplification = -1.0;

    protected:
        double maxCurrent = 0.0;

        bool isOutputContiguous() override { return false; }

        /**
         * Loads all saturation parameters except 'type' and 'mechanism'
         * The routine shall be run by the process with application rank 0
         *
         * @param source the loading source
         */
        void loadParameterList(const param::Object& source) override;

        /**
         * Loads all saturation parameters except 'type', 'mechanism', 'dark_current', 'amplification'
         * The routine shall be run by the process with application rank 0
         *
         * @param source representation of the corresponding JS object
         */
        virtual void loadSaturationParameterList(const param::Object& source) = 0;

        /**
         * Broadcasts all saturation parameters except 'type' and 'mechanism'
         * This is a collective routine relatively to the application communicator
         */
        void broadcastParameterList() override;

        /**
         * Broadcasts all saturation parameters except 'type', 'mechanism', 'dark_current', 'amplification'
         * This is a collective routine relatively to the application communicator
         */
        virtual void broadcastSaturationParameterList() = 0;

        /**
         * Adjusts the saturation parameter
         *
         * @param name parameter name
         * @param pvalue pointer to the parameter value
         */
        void setParameter(const std::string& name, const void* pvalue) override;

        /**
         * Adjusts saturation parameter except 'dark_current', 'amplification'
         *
         * @param name parameter name
         * @param pvalue pointer to the parameter value
         */
        virtual void setSaturationParameter(const std::string& name, const void* pvalue) = 0;


        /**
         * Provides individual initialization routines
         */
        virtual void initializeStimulusSaturation() = 0;

        /**
         * returns the saturation output for a given saturation input
         *
         * @param saturationInput input to the saturation block in nA
         * @return output from the saturation block in nA
         */
        virtual double getSaturationOutput(double saturationInput) = 0;

        void finalizeProcessor(bool destruct = false) noexcept override;

    public:
        /**
         * Creates new stimulus saturation
         *
         * @param comm general communicator for the model
         */
        explicit StimulusSaturation(mpi::Communicator& comm): Equation(comm) {};

        ~StimulusSaturation() override {
            finalizeProcessor(true);
        }

        /**
         * Creates a certain stimulus saturation
         *
         * @param comm communicator for which the saturation shall be created. Shall be the same as the stimulus
         * communicator
         * @param mechanism saturation mechanism
         * @return pointer to the stimulus saturation object
         */
        static StimulusSaturation* createStimulusSaturation(mpi::Communicator& comm, const std::string& mechanism);

        /**
         *
         * @return input current to the LGN or V1 that corresponds to pixels with value 0.0
         */
        [[nodiscard]] double getDarkCurrent() const { return darkCurrent; }

        /**
         *
         * @return stimulus amplification (how the input current will change if we shift pixel value from 0.0 to 1.0
         */
        [[nodiscard]] double getStimulusAmplitifacation() const { return stimulusAmplification; }

        /**
         *
         * @return maximum current
         */
        [[nodiscard]] double getMaxCurrent() { return maxCurrent;}

        class negative_stimulus_amplification: public simulation_exception{
        public:
            [[nodiscard]] const char* what() const noexcept override {
                return "Stimulus amplification is zero or negative";
            }
        };

        class negative_max_current: public simulation_exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "Maximum input current during the stimulus saturation shall not be negative or zero";
            }
        };

        /**
         *
         * @param value sets an input current to the LGN that corresponds to 0.0
         */
        void setDarkCurrent(double value) { darkCurrent = value; }

        /**
         * Sets the stimulus amplification
         *
         * @param value change in the input current that corresponds to the shift in the pixel value from 0.0 to 1.0
         */
        void setStimulusAmplification(double value) {
            if (value > 0){
                stimulusAmplification = value;
            } else {
                throw negative_stimulus_amplification();
            }
        }

        /**
         * Sets the maximum current for the saturation. This parameter will not apply to all saturations
         *
         * @param value maximum current in nA
         */
        void setMaxCurrent(double value){
            if (value > 0){
                maxCurrent = value;
            } else {
                throw negative_max_current();
            }
        }

        /**
         * Sets the input stimulus, allocates the output matrix, and runs initializeStimulusSaturation()
         *
         * @throws an exception if the saturation has input procesors or the stimulus has not been initialized
         */
        void initialize() override;

        /**
         * Updates the saturation
         *
         * @param time current time in ms
         */
        void update(double time) override;

        class wrong_input: public std::exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "There shall not be any input processors before the initialization of the stimulus saturation. "
                       "All necessary input processors will be added during the initialization routine";
            }
        };
    };

}


#endif //MPI2_STIMULUSSATURATION_H
