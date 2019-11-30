//
// Created by serik1987 on 30.11.2019.
//

#include "JobBuilder.h"
#include "../log/output.h"

namespace job{


    void JobBuilder::loadParameterList(const param::Object &source) {
        job_name = source.getStringField("mechanism");
        job = Job::createJob(comm, job_name);
        job->loadParameters(source);
    }

    void JobBuilder::broadcastParameterList() {
        auto& app = Application::getInstance();
        app.broadcastString(job_name, 0);
        if (app.getAppCommunicator().getRank() != 0){
            job = Job::createJob(comm, job_name);
        }
        job->broadcastParameters();
    }
}