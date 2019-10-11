#include <iostream>
#include <windows.h>

#include "../mpi/App.h"
#include "../mpi/Intercommunicator.h"
#include "../mpi/SuperIntercommunicator.h"

int main(int argc, char* argv[]) {

    mpi::App app(&argc, &argv);
    mpi::Communicator& comm = app.getAppCommunicator();
    int rank = comm.getRank();
    int numprocs = comm.getProcessorNumber();

    /* First, we shall create the local communicators */
    int membershipKey = rank % 2;
    mpi::Communicator localComm = comm.split(membershipKey, rank);

    /* Then, let's create intercommunicator */
    int remoteRank;
    if (membershipKey == 0){
        remoteRank = 1;
    } else {
        remoteRank = 0;
    }
    mpi::Intercommunicator interComm(localComm, 0, comm, remoteRank, 1);


    /* Let's test */
    int data = rank;
    if (rank == 0) {
        data = 100;
        interComm.send(&data, 1, MPI_INT, 0, 0);
        interComm.send(&data, 1, MPI_INT, 1, 1);
    }

    if (membershipKey == 1) {
        MPI_Status status;
        status = interComm.recv(&data, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG);

        /* Printing the result */
        app << "Individual operations test\n";
        app << "Source process: " << status.MPI_SOURCE << "\n";
        app << "Message tag: " << status.MPI_TAG << "\n";
    }

    comm.barrier();

    app << "Your data: " << data << "\n";
    app << "Local size: " << interComm.getProcessorNumber() << "\n";
    app << "Remote size: " << interComm.getRemoteSize() << "\n";
    app << "Local rank: " << interComm.getRank() << "\n";


    /* Test of the collectives */
    bool higher = membershipKey == 1;
    mpi::SuperIntercommunicator super(interComm, higher);
    if (rank == 0){
        data = 1000;
    }
    super.broadcast(&data, 1, MPI_INT, 1);

    app << "YOUR DATA: " << data << "\n";


    return 0;
}