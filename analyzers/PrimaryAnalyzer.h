//
// Created by serik1987 on 01.12.2019.
//

#ifndef MPI2_PRIMARYANALYZER_H
#define MPI2_PRIMARYANALYZER_H

#include "Analyzer.h"
#include "../models/Layer.h"

namespace analysis {

    /**
     * A base class for all analyzers that receive the data from layer, not other analyzers
     */
    class PrimaryAnalyzer: virtual public Analyzer {
    private:
        net:: Layer* source = nullptr;

    protected:
        void loadSource() override;

        mpi::Communicator& getInputCommunicator() override {
            return source->getCommunicator();
        };

    public:
        explicit PrimaryAnalyzer(mpi::Communicator& comm): Analyzer(comm) {};

        static PrimaryAnalyzer* createPrimaryAnalyzer(mpi::Communicator& comm, const std::string& mechanism);

        data::Matrix& getSource() override { return source->getOutputData()->getOutput(); }

        bool isInputAcceptable() override { return true; }
    };

}


#endif //MPI2_PRIMARYANALYZER_H
