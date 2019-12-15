//
// Created by serik1987 on 03.12.2019.
//

#ifndef MPI2_SECONDARYVSDANALYZER_H
#define MPI2_SECONDARYVSDANALYZER_H

#include "VsdAnalyzer.h"
#include "SecondaryAnalyzer.h"

namespace analysis {

    /**
     * A base class for all VSD analyzers that are attached to another VSD analyzers
     */
    class SecondaryVsdAnalyzer: public VsdAnalyzer, public SecondaryAnalyzer {
    private:
        double dt = -1.0;

    protected:
        void checkSource(Analyzer* source) override;

    public:
        explicit SecondaryVsdAnalyzer(mpi::Communicator& comm): VsdAnalyzer(comm), SecondaryAnalyzer(comm),
            Analyzer(comm) {};

        /**
         *
         * @return matrix width in pixels
         */
        [[nodiscard]] int getMatrixWidth() override;

        /**
         *
         * @return matrix height in pixels
         */
        [[nodiscard]] int getMatrixHeight() override;

        /**
         *
         * @return matrix width in um or any other units
         */
        [[nodiscard]] double getMatrixWidthUm() override;

        /**
         *
         * @return matrix height in um or any other units
         */
        [[nodiscard]] double getMatrixHeightUm() override;

        /**
         *
         * @return acquisition step in ms
         */
        [[nodiscard]] double getAcquisitionStep() const override { return dt; }

        class incorrect_input_analyzer: public std::exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "The source to the VSD analyzer may be only VSD analyzer";
            }
        };
    };

}


#endif //MPI2_SECONDARYVSDANALYZER_H
