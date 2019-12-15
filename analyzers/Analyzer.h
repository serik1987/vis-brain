//
// Created by serik1987 on 01.12.2019.
//

#ifndef MPI2_ANALYZER_H
#define MPI2_ANALYZER_H

#include "../processors/Processor.h"
#include "../jobs/Job.h"

namespace job{
    class Job;
}

namespace analysis {

    /**
     * A base class for all analyzers
     */
    class Analyzer: public equ::Processor {
    private:
        std::string source_name;
        mpi::Communicator* c = nullptr;
        job::Job* parent_job = nullptr;

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

        /**
         * Looks for an appropriate source of the analyzers and sets one of its private fields to
         * the source of this analyzer.
         *
         * The method doesn't look for a source matrix. This just looks for a source analyzer or
         * source layer. The output matrix of the layer may be changed at each iteration.
         */
        virtual void loadSource() = 0;

        /**
         *
         * @return reference to the communicator that shall be used for creating output matrices
         */
        virtual mpi::Communicator& getInputCommunicator() = 0;

        virtual void initializeAnalyzer() = 0;

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

        /**
         *
         * @return full address of the source
         */
        [[nodiscard]] const std::string& getSourceName() const { return source_name; }

        /**
         * Sets the source name, but doesn't set the source. This parameter can't be adjusted through
         * the job
         *
         * @param name the source name
         */
        void setSourceName(const std::string& name) { source_name = name; }

        class invalid_analyzer_source: public std::exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "Layer or analyzer pointed out as a source is incorrect or not found";
            }
        };

        /**
         *
         * @return the source data to analyze
         */
        virtual data::Matrix& getSource() = 0;

        /**
         *
         * @param time iteration time at a current timestamp
         * @return true if the analyzer is ready for update at this iteration, false
         * if the update shall be skipped
         */
        virtual bool isReady(double time)  = 0;

        /**
         *
         * @return true if secondary analyzer can be attached to this analyzer
         */
        virtual bool isInputAcceptable() = 0;

        /**
         *
         * @return reference to the main communicator
         */
        mpi::Communicator& getMainCommunicator() { return *c;}

        /**
         * Intiializes the processor
         */
        void initialize() override;

        /**
         *
         * @return the job that created this analyzer
         */
        job::Job* getCurrentJob() { return parent_job; }

        /**
         * To be launched by the job's createAnalyzers
         *
         * @param value analyzer values
         */
        void setJob(job::Job* value) { parent_job = value;}

    };

}


#endif //MPI2_ANALYZER_H
