//
// Created by serik1987 on 30.11.2019.
//

#include "Job.h"
#include "SingleRunJob.h"
#include "../log/output.h"

namespace job{

    void Job::loadParameterList(const param::Object &source) {
        logging::info("Single-run job");
        setOutputFilePrefix(source.getStringField("output_file_prefix"));
        logging::info("Output file prefix: " + getOutputFilePrefix());
        loadJobParameters(source);
    }

    void Job::broadcastParameterList() {
        Application::getInstance().broadcastString(output_file_prefix, 0);
        broadcastJobParameters();
    }

    Job *Job::createJob(mpi::Communicator &comm, const std::string &job_type) {
        Job* job = nullptr;

        if (job_type == "single-run"){
            job = new SingleRunJob(comm);
        }

        return job;
    }
}