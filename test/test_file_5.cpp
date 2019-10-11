#include "../mpi/App.h"
#include "../mpi/File.h"

#include <iostream>

int main(int argc, char* argv[]){
    mpi::App app(&argc, &argv);
    mpi::Communicator& comm = app.getAppCommunicator();
    int rank = comm.getRank();

    const int rows = 1000;
    const int cols = 1000;

    const int dy = 2;
    const int dx = 3;

    int local_rows = ceil((double)rows/dy);
    int local_cols = ceil((double)cols/dx);

    const double sigma = 100.0;

    int I = rank / dx;
    int J = rank % dx;

    int iStart = I * local_rows;
    int jStart = J * local_cols;

    if (I == 1){
        local_rows = 1000 - iStart;
    }

    if (J == 2){
        local_cols = 1000 - jStart;
    }

    double data[local_rows * local_cols];

    for (int index = 0; index < local_rows * local_cols; index++){
        int local_i = index / local_cols;
        int local_j = index % local_cols;
        int i = iStart + local_i;
        int j = jStart + local_j;
        double y = (i-500)/10.0;
        double x = (j-500)/10.0;
        data[index] = exp(-x*x/2/sigma-y*y/2/sigma);
    }

    int size = 6;
    int ndims = 2;
    int gsizes[] = {rows, cols};
    int distribs[] = {MPI_DISTRIBUTE_BLOCK, MPI_DISTRIBUTE_BLOCK};
    int dargs[] = {MPI_DISTRIBUTE_DFLT_DARG, MPI_DISTRIBUTE_DFLT_DARG};
    int psizes[] = {dy, dx};
    mpi::ArrayDatatype ftype(MPI_DOUBLE, size, rank, ndims, gsizes, distribs, dargs, psizes);
    mpi::File file(comm, "sample.dat", MPI_MODE_WRONLY | MPI_MODE_CREATE);
    file.setView(0, MPI_DOUBLE, ftype);
    file.writeAll(data, local_cols * local_rows, MPI_DOUBLE);

    return 0;
}
