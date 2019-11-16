//
// Created by serik1987 on 16.11.2019.
//

#ifndef MPI2_COMPLEXSTIMULUS_H
#define MPI2_COMPLEXSTIMULUS_H

#include "Stimulus.h"

namespace stim {

    /**
     * A base class for all stimuli that will be derived by application
     * of some operations to a set of more simple stimulus
     */
    class ComplexStimulus: public Stimulus {
    private:
        std::vector<std::string> mechanism_names;

    protected:
        double recordLength = 0.0;

        /**
         * Loads all children stimuli, loads their parameters and calls loadAllComplexStimulusParameters
         * The routine shall be run by the process with rank 0 within the application communicator
         *
         * @param source JS object to load
         */
        void loadStimulusParameters(const param::Object& source) override;

        /**
         * Creates child stimuli in all processes with rank different from zero,
         * broadcasts parameters from all child stimuli and calls broadcastComplexStimuulusParameters
         * This is a collective routine. It shall be run by all processes containing in the application communicator
         */
        void broadcastStimulusParameters() override;

        /**
         * Sets all complex stimulus parameters except:
         * 'grid_x', 'grid_y', 'size_x', 'size_y', 'luminance', 'contrast'
         *
         * @param name either name of the complex stimulus parameter or string like
         * "idx.param" where idx is an index of the child stimulus and param is a parameter of the complex stimulus
         * @param pvalue
         */
        void setStimulusParameter(const std::string& name, const void* pvalue) override;

        /**
         * Initializes all child stimuli and runs initializeComplexStimulus routine
         * This doesn't initialize output matrix because the matrix has already been initialized by the initialize()
         * routine
         * This is a collective routine
         */
        void initializeStimulus() override;

        /**
         * Initializes common entities of the complex stimulus, shall adjust the recordLength option
         */
        virtual void initializeComplexStimulus() = 0;

        /**
         * Finalizes all child stimuli
         * This is a collective routine
         *
         * @param destruct true if you want to delete all input processors, false otherwise
         */
        void finalizeProcessor(bool destruct = false) noexcept;

        /**
         * Finalizes common entities of the complex stimulus
         *
         * @param destruct false everywhere except for destructor
         */
        virtual void finalizeComplexStimulus(bool destruct) = 0;

        /**
         * Loads all parameters of the complex stimulus except:
         * 'type', 'mechanism', 'grid_x', 'grid_y', 'size_x', 'size_y', 'luminance',
         * 'contrast' and 'content'
         * This routine shall be run by the process with rank 0 in the application communicator
         *
         * @param source param::Object instance
         */
        virtual void loadComplexStimulusParameters(const param::Object& source) = 0;

        /**
         * Broadcasts all parameters of the complex stimulus except:
         * 'type', 'mechanism', 'grid_x', 'grid_y', 'size_x', 'size_y', 'luminance',
         * 'contrast' and 'content'
         * This is a collective routine. It shall be run by all processes containing in the application communicator
         * (application communicator is so called MPI_COMM_WORLD).
         */
        virtual void broadcastComplexStimulusParameters() = 0;

        /**
         * Sets an arbitrary parameter of the complex stimulus except:
         * 'grid_x', 'grid_y', 'size_x', 'size_y', 'luminance', 'contrast' or any parameter of the child stimulus
         *
         * @param name parameter name
         * @param pvalue pointer to the parameter value
         */
        virtual void setComplexStimulusParameter(const std::string& name, const void* pvalue) = 0;

    public:
        explicit ComplexStimulus(mpi::Communicator& comm): Stimulus(comm) {};

        ~ComplexStimulus() override {
            finalizeProcessor(true);
        }

        /**
         * Creates complex stimulus using its name
         *
         * @param comm communicator of the stimulus
         * @param name stimulus minor name
         * @return
         */
        static ComplexStimulus* createComplexStimulus(mpi::Communicator& comm, const std::string& name);

        /**
         * If the stimulus is not initialize()'d the method returns 0.0. However, if the stimulus is initialize()'d
         * this returns total duration of all the experiment
         *
         * @return total length of the experiment in ms or 0.0
         */
        [[nodiscard]] double getRecordLength() const override { return recordLength; }

        class stimulus_dimensions_mismatch: public simulation_exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "Stimulus dimensions of the child stimulus are not the same as stimulus dimensions of the"
                       "complex stimulus";
            }
        };

        class stimulus_size_mismatch: public simulation_exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "Stimulus size of the child stimulus is not the same as stimulus size of the complex stimulus";
            }
        };
    };

}


#endif //MPI2_COMPLEXSTIMULUS_H
