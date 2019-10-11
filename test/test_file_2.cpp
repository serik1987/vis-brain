#include "../mpi/App.h"
#include "../mpi/File.h"

int main(int argc, char* argv[]){
    mpi::App app(&argc, &argv);
    mpi::Communicator& comm = app.getAppCommunicator();
    int numprocs = comm.getProcessorNumber();
    int rank = comm.getRank();

    mpi::File file(comm, "sample.dat", MPI_MODE_RDONLY);
    int size = file.getSize();
    int startPosition = rank * size / numprocs;
    int readingPlan = size/numprocs;
    unsigned char x;

    app.lock();
    app << "File has been opened successfully\n";
    app << "File size: " << size << " bytes\n";
    app << "Starting position: " << startPosition << " bytes\n";
    app << "Reading plan: " << readingPlan << " bytes\n";
    app << "Reading result: ";
    file.seek(startPosition);
    for (int i = 0; i < readingPlan; i++){
        file.read(&x, 1, MPI_UNSIGNED_CHAR);
        app << (int)x << " ";
    }
    app << "\n";
    app.unlock();

}