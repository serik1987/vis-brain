#include "mpi/App.h"
#include "mpi/File.h"

int main(int argc, char* argv[]){
    mpi::App app(&argc, &argv);
    mpi::Communicator& comm = app.getAppCommunicator();
    int numprocs = comm.getProcessorNumber();
    int rank = comm.getRank();

    int buf[1000];
    for (int i=0; i<1000; i++){
        buf[i] = i;
    }

    mpi::File file(comm, "sample.dat", MPI_MODE_WRONLY | MPI_MODE_CREATE);
    mpi::ContiguiousDatatype contig(MPI_INT, 2);
    contig.disableAutocommit();
    MPI_Aint lb = 0;
    MPI_Aint extent = 6 * sizeof(int);
    MPI_Aint disp = 5*sizeof(int);
    mpi::ResizedDatatype filetype(contig, lb, extent);
    file.setView(disp, MPI_INT, filetype);
    file.write(buf, 1000, MPI_INT);

    app.lock();
    app << "Total number of autocommits: " << mpi::autocommits << "\n";
    app.unlock();

    
}