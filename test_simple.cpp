#include <iostream>

#include "mpi/App.h"

int main(int argc, char* argv[]) {

    mpi::App app(&argc, &argv);
    mpi::Communicator comm = app.getAppCommunicator();
    int rank = comm.getRank();
    int numprocs = comm.getProcessorNumber();
    int n = 100;

    app << "Enter the number of intervals";
    app >> n;

    /* Vector/matrix machinery -> after STL study */
    double h = 1.0/n;
    int n_local = n / numprocs;
    int left = n_local * rank;
    int right = left + n_local-1;
    if (rank == numprocs-1){
        right = n-1;
    }

    /* Main working algorithm */
    double local_sum = 0;
    for (int i = left; i <= right; i++){
        double x = h * (double(i)+0.5);
        local_sum += (4.0 / (1.0 + x*x));
    }
    local_sum *= h;
    double sum = 0.0;
    comm.reduce(&local_sum, &sum, 1, MPI_DOUBLE, MPI_SUM, 0);
    /* After main working algorithm */

    app << "Computations has done.\n";

    if (rank == 0) {
        app << "Sum: " << sum << "\n";
    }


    return 0;
}