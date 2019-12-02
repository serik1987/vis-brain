//
// Created by serik1987 on 01.12.2019.
//

#ifndef MPI2_VSDANALYZER_H
#define MPI2_VSDANALYZER_H

#include "Analyzer.h"

namespace analysis {

    class VsdAnalyzer: virtual public Analyzer {
    protected:

        /**
         * Performs all other initialization routines except creation of the VSD matrix
         */
        virtual void initializeVsd() = 0;

    public:
        /**
         *
         * @param comm communicator that is responsible for information processing for a certain analyser
         */
        explicit VsdAnalyzer(mpi::Communicator& comm): Analyzer(comm) {};

        /**
         * Intiializes the processor
         */
        void initialize() override;
    };

}


#endif //MPI2_VSDANALYZER_H
