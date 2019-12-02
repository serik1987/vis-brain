//
// Created by serik1987 on 01.12.2019.
//

#ifndef MPI2_PRIMARYANALYZER_H
#define MPI2_PRIMARYANALYZER_H

#include "Analyzer.h"

namespace analysis {

    class PrimaryAnalyzer: virtual public Analyzer {
    public:
        explicit PrimaryAnalyzer(mpi::Communicator& comm): Analyzer(comm) {};

        static PrimaryAnalyzer* createPrimaryAnalyzer(mpi::Communicator& comm, const std::string& mechanism);
    };

}


#endif //MPI2_PRIMARYANALYZER_H
