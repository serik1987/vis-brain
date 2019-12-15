//
// Created by serik1987 on 01.12.2019.
//

#ifndef MPI2_VSDANALYZER_H
#define MPI2_VSDANALYZER_H

#include "Analyzer.h"

namespace analysis {

    /**
     * A base class for all analyzers that record and process the simulation VSD data
     */
    class VsdAnalyzer: virtual public Analyzer {
    protected:

        /**
         * Performs all other initialization routines except creation of the VSD matrix
         */
        virtual void initializeVsd() = 0;

        void initializeAnalyzer() override;

    public:
        /**
         *
         * @param comm communicator that is responsible for information processing for a certain analyser
         */
        explicit VsdAnalyzer(mpi::Communicator& comm): Analyzer(comm) {};

        /**
         *
         * @return the matrix width in pixels
         */
        virtual int getMatrixWidth() = 0;

        /**
         *
         * @return the matrix height in pixels
         */
        virtual int getMatrixHeight() = 0;

        /**
         *
         * @return matrix width in um or any other pixels
         */
        virtual double getMatrixWidthUm() = 0;

        /**
         *
         * @return matrix height in um or any other pixels
         */
        virtual double getMatrixHeightUm() = 0;

        /**
         *
         * @return acquisition step in ms
         */
        virtual double getAcquisitionStep() const = 0;
    };

}


#endif //MPI2_VSDANALYZER_H
