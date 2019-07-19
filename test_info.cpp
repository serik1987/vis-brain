

#include "mpi/App.h"
#include "mpi/Info.h"
#include "mpi/File.h"

int main(int argc, char* argv[]){
    mpi::App app(&argc, &argv);
    mpi::Communicator& world = app.getAppCommunicator();
    int rank = world.getRank();

    mpi::File file(world, "sample.dat", MPI_MODE_RDONLY);
    mpi::Info info;
    info = file.getInfo();
    std::cout << info << std::endl;
}
