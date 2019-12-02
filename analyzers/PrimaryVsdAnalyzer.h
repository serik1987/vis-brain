//
// Created by serik1987 on 01.12.2019.
//

#ifndef MPI2_PRIMARYVSDANALYZER_H
#define MPI2_PRIMARYVSDANALYZER_H

#include "PrimaryAnalyzer.h"
#include "VsdAnalyzer.h"

namespace analysis {

    class PrimaryVsdAnalyzer: public PrimaryAnalyzer, public VsdAnalyzer {
    protected:
        [[nodiscard]] std::string getProcessorName() override { return "analysis::PrimaryVsdAnalyzer"; };
        void loadAnalysisParameters(const param::Object& source) override;
        void broadcastAnalysisParameters() override;
        void setAnalyzerParameters(const std::string& name, const void* pvalue) override;
        bool isOutputContiguous() override { return false; }
        void initializeVsd() override;

        void finalizeProcessor(bool destruct = false) noexcept override;

    public:
        explicit PrimaryVsdAnalyzer(mpi::Communicator& comm): PrimaryAnalyzer(comm), VsdAnalyzer(comm),
            Analyzer(comm) {};

        ~PrimaryVsdAnalyzer() {
            finalizeProcessor(true);
        }

        /**
         * The method runs each timestamp (after the integration method will be applied
         * and is used to keep the analyser data up to date
         *
         * @param time current time in ms
         */
        void update(double time) override;
    };

}


#endif //MPI2_PRIMARYVSDANALYZER_H
