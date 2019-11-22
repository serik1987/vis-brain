//
// Created by serik1987 on 20.11.2019.
//

#include "TemporalKernel.h"
#include "OdeTemporalKernel.h"

namespace equ{

    TemporalKernel *TemporalKernel::createTemporalKernel(mpi::Communicator &comm, const std::string &mechanism_name,
            equ::Ode::SolutionParameters parameters) {
        TemporalKernel* result = nullptr;

        if (mechanism_name == "ode"){
            result = new OdeTemporalKernel(comm, parameters);
        } else {
            throw param::UnknownMechanism("equ:temporal_kernel." + mechanism_name);
        }
    }
}