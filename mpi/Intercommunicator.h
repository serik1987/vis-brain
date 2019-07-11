//
// Created by User on 09.07.2019.
//

#ifndef MPI2_INTERCOMMUNICATOR_H
#define MPI2_INTERCOMMUNICATOR_H

#include "Communicator.h"


namespace mpi {

    /**
     * See help on constructor for details
     */
    class Intercommunicator : public Communicator {
    private:
        Communicator* localComm;
        Communicator* remoteComm;
        int tag;

        int remoteSize = -1;

        void testCommunicator();
    public:
        Intercommunicator(): Communicator(MPI_COMM_NULL) {};

        Intercommunicator(MPI_Comm comm) = delete;

        /**
         * Creates intercommunicator. Intercommunicator provides data transmission between two different groups
         * of processes.
         * In order to use this faciity
         * a) create local communicators via Communicator::split of Group::createCommunicator in order to define
         * the process groups.
         * b) Put two groups each of which is represented by the local communicator comm
         * c) Construct the mpi::Intercommunicator object solely for all processes belong to these two groups
         *
         * Group A             <->                 Group B
         * Intercommunicator provides connection between Groups A and B.
         * When the constructor is called by the process belonging to the group A,
         * Group A represents the local communicator
         * Group B represents the remote communicator
         * When the constructor is called by the process belonging to the group B,
         * Group B represents the local communicator
         * Group A represents the remote communicator
         * In any way the parent communicator is a communicator that created both communicators corresponding
         * to groups A and B.
         *
         *
         * @param lc - the local communicator
         * @param lr - the process in the local communicator that shall be the process rank 0 in the local communicator
         * @param rc - the parent communicator
         * @param rr - the process in the parant communicator that shall be defined as rank 0 in the remote communicator
         * @param tag_ - rank of the intercommunicator. Different intercommunicators shall have different tags. If
         * two processes run the constructor with the same tag_ the same intercommunicator will be created during
         * these two different groups
         */
        Intercommunicator(Communicator& lc, int lr, Communicator& rc, int rr,
                int tag_): Communicator(MPI_COMM_NULL){
            localComm = &lc;
            remoteComm = &rc;
            tag = tag_;
            int errcode;
            if ((errcode = MPI_Intercomm_create(*lc, lr, *rc, rr, tag_, &comm)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            deletable = true;
        }

        Intercommunicator(const Intercommunicator& other) = delete;

        /**
         * Returns the size of the remote communicator
         *
         * @return the size of the remote group in the intercommunicator
         */
        int getRemoteSize(){
            if (remoteSize == -1){
                int errcode;
                if ((errcode = MPI_Comm_remote_size(comm, &remoteSize)) != MPI_SUCCESS){
                    throw_exception(errcode);
                }
            }
            return remoteSize;
        }

        Group getRemoteGroup(){
            MPI_Group group;
            int errcode;
            if ((errcode  = MPI_Comm_remote_group(comm, &group)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            return Group(group);
        }
    };


}

#endif //MPI2_INTERCOMMUNICATOR_H
