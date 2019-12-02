//
// Created by serik1987 on 01.12.2019.
//

#ifndef MPI2_ANALYSISBUILDER_H
#define MPI2_ANALYSISBUILDER_H

#include "../param/Loadable.h"
#include "../mpi/Communicator.h"
#include "Analyzer.h"

namespace analysis {

    class AnalysisBuilder: public param::Loadable {
    private:
        mpi::Communicator& comm;
        std::string mechanism;
        Analyzer* analyzer = nullptr;

    protected:
        [[nodiscard]] const char* getObjectType() const noexcept override { return "processor"; };
        void loadParameterList(const param::Object& source) override;
        void broadcastParameterList() override;
        void setParameter(const std::string& name, const void* pvalue) override {
            throw param::IncorrectParameterName(name, "analysis builder");
        };

    public:
        explicit AnalysisBuilder(mpi::Communicator& c): comm(c) {};

        [[nodiscard]] Analyzer* getAnalyzer() { return analyzer; }

        class incorrect_analyzer: public std::exception{
        public:
            [[nodiscard]] const char* what() const noexcept override {
                return "the processor substituted as analyzer is not an analyer (wrong mechanism selected)";
            }
        };
    };

}


#endif //MPI2_ANALYSISBUILDER_H
