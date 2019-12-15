//
// Created by serik1987 on 30.11.2019.
//

#ifndef MPI2_JOB_H
#define MPI2_JOB_H

#include <unordered_map>
#include "../param/Loadable.h"
#include "../mpi/Communicator.h"
#include "../analyzers/Analyzer.h"
#include "../compile_options.h"
#include "../sys/security.h"

namespace analysis{
    class Analyzer;
}

namespace job {

    /**
     * A base class for all jobs
     */
    class Job: public param::Loadable {
    public:
        class set_job_parameter_error: public std::exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "can't set the job parameters arbitrarily";
            }
        };

    private:
        mpi::Communicator& comm;
        std::string output_file_prefix;

        void createAnalyzers(const param::Object& source);
        void broadcastAnalyzers();

        std::list<analysis::Analyzer*> analysis_list;
        std::list<std::string> analysis_names;
        std::unordered_map<std::string, analysis::Analyzer*> analysis_map;
        std::list<std::string> analysis_mechanisms;

    protected:
        [[nodiscard]] const char* getObjectType() const noexcept override {return "job"; };
        void loadParameterList(const param::Object& source) override;
        void broadcastParameterList() override;
        void setParameter(const std::string& name, const void* pvalue) override {
            throw set_job_parameter_error();
        };

        /**
         * Loads all job parameters except: type, mechanism, output_folder_prefix
         *
         * @param source object that contains all job parameters
         */
        virtual void loadJobParameters(const param::Object& source) = 0;

        /**
         * Broadcasts all job parameters except: type, mechanism, output_folder_prefix
         */
        virtual void broadcastJobParameters() = 0;

        void initializeAnalyzers();

        void updateAnalyzers(double time);

        void finalizeAnalyzers();

    public:
        explicit Job(mpi::Communicator& c): comm(c) {};

        ~Job() override;

        /**
         *
         * @return the Job communicator
         */
        [[nodiscard]] mpi::Communicator& getJobCommunicator() { return comm; }

        /**
         * Creates one of the available descendant classes
         *
         * @param comm reference to the communicator
         * @param job_type value of the 'mechanism' field
         * @return pointer to the job created
         */
        static Job* createJob(mpi::Communicator& comm, const std::string& job_type);

        /**
         *
         * @return output file prefix
         */
        const std::string& getOutputFilePrefix() const { return output_file_prefix; }

        /**
         * Sets the output file prefix
         *
         * @param prefix new prefix
         */
        void setOutputFilePrefix(const std::string& prefix) {
#if SERVER_BUILD==1
            sys::security_check("output_file_prefix", prefix);
#endif
            output_file_prefix = prefix;
        }

        /**
         * Starts the job
         */
        virtual void start() = 0;

        /**
         * Looks for analysis by the full name
         *
         * @param name analyzer name
         * @return pointer to the analyzer
         */
        virtual analysis::Analyzer* getAnalyzer(const std::string& name);
    };

}


#endif //MPI2_JOB_H
