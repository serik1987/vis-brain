//
// Created by serik1987 on 28.11.2019.
//

#ifndef MPI2_DISTRIBUTOR_H
#define MPI2_DISTRIBUTOR_H

#include "Method.h"
#include "../models/AbstractNetwork.h"
#include "../processors/State.h"

namespace method{

    /**
     * A base class for all distributors.
     * A distributor distributes all layers among the processes by setting the personal communicator
     * to all processors.
     *
     * WARNING. All communicators will be automatically destroyed after the distributor will be deleted or
     * out of scope. Since that time, any simulation will be impossible, the application will not work.
     */
    class Distributor{
    private:
        mpi::Communicator& comm;
        Method& method;

    public:
        /**
         * Creates new distributor
         *
         * @param comm reference to the communicator for all distributors. All other communicators will be child
         * to this certain communicator
         * @param method reference to the ODE solution method
         */
        Distributor(mpi::Communicator& comm, Method& method): comm(comm), method(method) {};

        virtual ~Distributor() = default;

        /**
         *
         * @return the parent communicator to all layer communicators
         */
        [[nodiscard]] mpi::Communicator& getCommunicator() { return comm; }

        /**
         *
         * @return reference to the ODE solution method
         */
        [[nodiscard]] Method& getMethod() { return method; }

        /**
         * Distributes all layers in the network among processes
         *
         * @param network
         */
        virtual void apply(net::AbstractNetwork& network) = 0;

        /**
         *
         * @return communicator that shall be assigned to the stimulus
         */
        virtual mpi::Communicator& getStimulusCommunicator() = 0;


    };

}

#endif //MPI2_DISTRIBUTOR_H
