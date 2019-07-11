//
// Created by User on 09.07.2019.
//

#ifndef MPI2_SUPERINTERCOMMUNICATOR_H
#define MPI2_SUPERINTERCOMMUNICATOR_H

#include "Intercommunicator.h"

namespace mpi {

    /**
     * Represents the intracommunicator where all collective routines can be transmit information between the
     * processes only
     */
    class SuperIntercommunicator : public Intercommunicator {
    public:
        SuperIntercommunicator(Communicator& lc, int lr, Communicator& rc, int rr,
        int tag_) = delete;

        /**
         * Creates from Intercommunicator the other one that is available for collective operation.
         *
         * General information flow during the collective operations
         * Group A -> Group B
         *
         * @param ic - the parent intercommunicator
         * @param higher - put true if the constructor is called from the group B or false if it called from the
         * group A
         */
        SuperIntercommunicator(Intercommunicator& ic, bool higher){
            int errcode;
            int h = higher;
            if ((errcode = MPI_Intercomm_merge(*ic, h, &comm)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            deletable = true;
        }
    };

}

#endif //MPI2_SUPERINTERCOMMUNICATOR_H
