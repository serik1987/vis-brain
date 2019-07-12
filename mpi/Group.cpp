//
// Created by User on 02.07.2019.
//

#define GROUP_OBJECT_CODE

#include "Group.h"

#include <iostream>

namespace mpi {

    Group::Group(const Communicator *c) {
        comm = c;
        int errcode;
        if ((errcode = MPI_Comm_group(comm->operator*(), &group)) != MPI_SUCCESS) {
            throw_exception(errcode);
        }
    }

    Communicator Group::createCommunicator() {
        /* Creates a communicator based on the group
         *
         */
        int errcode;
        MPI_Comm newcomm = 0;
        if ((errcode = MPI_Comm_create(**comm, group, &newcomm)) != MPI_SUCCESS) {
            throw_exception(errcode);
        }
        mpi::Communicator result(newcomm);
        return result;
    }
}