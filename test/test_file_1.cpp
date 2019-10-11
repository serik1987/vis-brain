#include "../mpi/App.h"
#include "../mpi/File.h"
#include "../mpi/Status.h"

mpi::Status getStatus(){
    mpi::Status status;
    std::cout << "SUBROUTINE STATUS: " << (MPI_Status*)status << "\n";
    return status;
}

int main(int argc, char* argv[]){
    mpi::App app(&argc, &argv);
    mpi::Communicator& comm = app.getAppCommunicator();
    int numprocs = comm.getProcessorNumber();
    int rank = comm.getRank();

    mpi::File file(comm, "sample.dat", MPI_MODE_WRONLY | MPI_MODE_CREATE);

    int position_start = rank * 256 / numprocs;
    int position_finish = position_start + 256 / numprocs;

    file.seek(position_start);

    int n;

    for (unsigned char x = (unsigned char) position_start, n=0; n < 256/numprocs; ++x, ++n) {
        file.write(&x, 1, MPI_UNSIGNED_CHAR);
        std::cout << (int) x << std::endl;
    }

}