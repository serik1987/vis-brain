//
// Created by serik1987 on 30.11.2019.
//

#ifndef MPI2_JOBBUILDER_H
#define MPI2_JOBBUILDER_H

#include "Job.h"
#include "../param/Loadable.h"

namespace job {

    class JobBuilder: public param::Loadable {
    private:
        Job* job = nullptr;
        std::string job_name;
        mpi::Communicator& comm;

    protected:
        [[nodiscard]] const char* getObjectType() const noexcept override { return "job"; }
        void loadParameterList(const param::Object& source) override;
        void broadcastParameterList() override;
        void setParameter(const std::string& name, const void* pvalue) override {
            throw Job::set_job_parameter_error();
        }

    public:
        explicit JobBuilder(mpi::Communicator& c): comm(c) {};

        /**
         *
         * @return job that has recently been constructed
         */
        [[nodiscard]] Job* getJob() { return job; }
    };
}


#endif //MPI2_JOBBUILDER_H
