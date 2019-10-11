#include "../mpi/App.h"
#include "../mpi/File.h"

static const int buf_size = 4096;
static const int blocklength = 16;

int main(int argc, char* argv[]){
    mpi::App app(&argc, &argv);
    mpi::Communicator& comm = app.getAppCommunicator();
    int numprocs = comm.getProcessorNumber();
    int rank = comm.getRank();

    int buffer[buf_size];
    for (int i=0; i < buf_size; i++){
        buffer[i] = (i + 1) * (rank + 1);
    }

    mpi::File file(comm, "sample.dat", MPI_MODE_WRONLY | MPI_MODE_CREATE);

    int stride = blocklength * numprocs;
    int count = buf_size / blocklength;
    mpi::VectorDatatype filetype(MPI_INT, blocklength, stride, count);

    int displ = rank * blocklength * sizeof(int);
    file.setView(displ, MPI_INT, filetype);

    file.writeAll(buffer, buf_size, MPI_INT);
}