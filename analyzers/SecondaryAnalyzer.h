//
// Created by serik1987 on 03.12.2019.
//

#ifndef MPI2_SECONDARYANALYZER_H
#define MPI2_SECONDARYANALYZER_H

#include "Analyzer.h"

namespace analysis {

    /**
     * A base class for all analyzers that are attached to another analyzers
     */
    class SecondaryAnalyzer: virtual public Analyzer {
    private:
        Analyzer* source = nullptr;
        double current_time = -1.0;
        bool ready;

    protected:
        void loadSource() override;
        mpi::Communicator& getInputCommunicator() override { return source->getMainCommunicator(); };

        /**
         * provides an additional source check, after the source has been found
         *
         * @param src the source processor itself
         */
        virtual void checkSource(Analyzer* src) = 0;

    public:
        explicit SecondaryAnalyzer(mpi::Communicator& comm): Analyzer(comm) {};

        static SecondaryAnalyzer* createSecondaryAnalyzer(mpi::Communicator& comm, const std::string& mechanism);

        /**
         *
         * @return the source data to analyze
         */
        data::Matrix& getSource() override { return source->getOutput(); }

        bool isReady(double time) override;

        class incorrect_source: public std::exception{
        private:
            std::string message;

        public:
            explicit incorrect_source(std::string name): std::exception() {
                message = name + " is not a valid source for the secondary analyzer";
            }

            [[nodiscard]] const char* what() const noexcept override{
                return message.c_str();
            }
        };

        class input_not_acceptable: public std::exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "The source analyzer doesn't have an output";
            }
        };
    };

}


#endif //MPI2_SECONDARYANALYZER_H
