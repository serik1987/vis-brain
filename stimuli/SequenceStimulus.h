//
// Created by serik1987 on 17.11.2019.
//

#ifndef MPI2_SEQUENCESTIMULUS_H
#define MPI2_SEQUENCESTIMULUS_H

#include "ComplexStimulus.h"

namespace stim {

    /**
     * Represents temporal sequence of more elementary stimuli
     */
    class SequenceStimulus: public ComplexStimulus {
    private:
        bool shuffle = false;
        int repeats = 0;
        std::string name;

        double singleRepeatLength = 0.0;
        double repeatStartTime = 0.0;
        int frameNumber = 0;
        double trialStartTime = 0.0;
        double trialLength = 0.0;
        std::vector<int>* indices = nullptr;
        int currentTrial = 0;
        int currentStimulus = 0;
        Stimulus* pstimulus;
        int repeatNumber = 0;

        void printProtocolLine();

    protected:
        [[nodiscard]] std::string getProcessorName() override { return "stim::SequenceStimulus"; }
        void loadComplexStimulusParameters(const param::Object& source) override;
        void broadcastComplexStimulusParameters() override;
        void setComplexStimulusParameter(const std::string& name, const void* pvalue) override;
        void initializeComplexStimulus() override;
        void finalizeComplexStimulus(bool destruct) override;

    public:
        explicit SequenceStimulus(mpi::Communicator& comm): ComplexStimulus(comm) {};

        ~SequenceStimulus() override {
            finalizeComplexStimulus(true);
        }

        class negative_repeats: public simulation_exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "Number of repeats of the stimulus sequence is zero or negative";
            }
        };

        /**
         *
         * @return true if all stimuli shall be shuffled before each repeat
         */
        [[nodiscard]] bool isShuffle() const { return shuffle; }

        /**
         * Defines whether stimuli in the sequence shall be shuffled before each repeat
         *
         * @param value true for yes, false for no
         */
        void setShuffle(bool value) { shuffle = value; }

        /**
         *
         * @return total number of all repeats
         */
        [[nodiscard]] int getRepeats() const { return repeats;}

        /**
         * Sets number of repeats for the sequence
         *
         * @param value number of repeats
         */
        void setRepeats(int value){
            if (value > 0){
                repeats = value;
            } else {
                throw negative_repeats();
            }
        }

        /**
         *
         * @return name of the sequence. This name will be used in stimulus protocols
         */
        [[nodiscard]] const std::string& getName() const{ return name; }

        /**
         * Sets the sequence name. This name will be used in stimulus protocols.
         *
         * @param protocol_name
         */
        void setName(const std::string& protocol_name) {
            name = protocol_name;
        }

        void update(double time) override;

    };

}


#endif //MPI2_SEQUENCESTIMULUS_H
