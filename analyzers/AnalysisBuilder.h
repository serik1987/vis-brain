//
// Created by serik1987 on 01.12.2019.
//

#ifndef MPI2_ANALYSISBUILDER_H
#define MPI2_ANALYSISBUILDER_H

#include "../param/Loadable.h"
#include "../mpi/Communicator.h"
#include "Analyzer.h"

namespace analysis {

    /**
     * Auxiliary class which objects are used to create any analyzer from the parameter object
     * and broadcast the whole analyzer
     *
     * The analyzer will be created through loadParameters and broadcastParameters method, after
     * that the pointer to the analyzer may be received through the getAnalyzer() method. This is
     * your duty to free the analyzer pointer
     */
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
        /**
         * Creating the analyzer
         *
         * @param c analysis communicator; this parameter is meaningless
         */
        explicit AnalysisBuilder(mpi::Communicator& c): comm(c) {};

        /**
         *
         * @return after the analyzer
         */
        [[nodiscard]] Analyzer* getAnalyzer() { return analyzer; }

        class incorrect_analyzer: public std::exception{
        public:
            [[nodiscard]] const char* what() const noexcept override {
                return "the processor substituted as analyzer is not an analyzer (wrong mechanism selected)";
            }
        };
    };

}


#endif //MPI2_ANALYSISBUILDER_H
