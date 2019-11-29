//
// Created by serik1987 on 28.11.2019.
//

#ifndef MPI2_EQUALDISTRIBUTOR_H
#define MPI2_EQUALDISTRIBUTOR_H

#include "Distributor.h"

namespace method {

    class EqualDistributor: public Distributor {
    public:
        EqualDistributor(mpi::Communicator& comm, Method& method): Distributor(comm, method) {};

        void apply(net::AbstractNetwork& network) override;

        mpi::Communicator& getStimulusCommunicator() override { return getCommunicator(); };
    };

}


#endif //MPI2_EQUALDISTRIBUTOR_H
