//
// Created by serik1987 on 30.11.2019.
//

#ifndef MPI2_SINGLERUNJOB_H
#define MPI2_SINGLERUNJOB_H

#include "Job.h"

namespace job {


    class SingleRunJob: public Job {
    protected:
        void loadJobParameters(const param::Object& source) override {};
        void broadcastJobParameters() override {};

    public:
        explicit SingleRunJob(mpi::Communicator& comm): Job(comm) {};

        /**
         * Starts the job
         */
        void start() override;
    };

}


#endif //MPI2_SINGLERUNJOB_H
