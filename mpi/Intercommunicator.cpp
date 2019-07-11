//
// Created by User on 09.07.2019.
//

#include "Intercommunicator.h"

void mpi::Intercommunicator::testCommunicator() {
    int flag;
    int errcode;
    if ((errcode = MPI_Comm_test_inter(comm, &flag)) != MPI_SUCCESS){
        throw_exception(errcode);
    }
    if (flag == 0){
        throw communicator_error();
    }
}