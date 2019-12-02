//
// Created by serik1987 on 01.12.2019.
//

#ifndef MPI2_ANALYZER_H
#define MPI2_ANALYZER_H

#include "../processors/Processor.h"

namespace analysis {

    class Analyzer: public equ::Processor {
    private:
        std::string source_name;

    protected:
        void loadParameterList(const param::Object& source) override;
        void broadcastParameterList() override;

        /**
         * Loads all analyzer parameters except: type, mechanism, source
         *
         * @param source
         */
        virtual void loadAnalysisParameters(const param::Object& source) = 0;

        /**
         * Broadcasts all analysis parameters except: type, mechanism, source
         */
        virtual void broadcastAnalysisParameters() = 0;

        /**
         * Sets anly analyzer parameter except: type, mechanism, source
         *
         * @param name parameter name
         * @param pvalue pointer to the parameter value
         */
        virtual void setAnalyzerParameters(const std::string& name, const void* pvalue) = 0;

    public:
        explicit Analyzer(mpi::Communicator& comm): Processor(comm) {};

        static Analyzer* createAnalyzer(mpi::Communicator& comm, const std::string& mechanism);

        /**
         * Sets the parameter by name
         *
         * @param name parameter name
         * @param pvalue pointer to the parameter value
         */
        void setParameter(const std::string& name, const void* pvalue) override;

    };

}


#endif //MPI2_ANALYZER_H
