#include "../mpi/App.h"
#include "../mpi/GraphCommunicator.h"

int main(int argc, char* argv[]){
    mpi::App app(&argc, &argv);
    mpi::Communicator& comm = app.getAppCommunicator();
    int numprocs = comm.getProcessorNumber();
    int rank = comm.getRank();

    int n;
    int sources[100];
    int degrees[100];
    int destinations[100];
    int weights[] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1};

    app << "Test of the graph # 2\n";

    if (rank == 0){
        n = 2;
        sources[0] = 2;
        sources[1] = 3;
        degrees[0] = 1;
        degrees[1] = 2;
        destinations[0] = 1;
        destinations[1] = 2;
        destinations[2] = 5;
    } else if (rank == 1){
        n = 2;
        sources[0] = 0;
        sources[1] = 1;
        degrees[0] = 3;
        degrees[1] = 0;
        destinations[0] = 3;
        destinations[1] = 4;
        destinations[2]= 1;
    } else if (rank == 2){
        n = 2;
        sources[0] = 4;
        sources[1] = 5;
        degrees[0] = 1;
        degrees[1] = 1;
        destinations[0] = 0;
        destinations[1] = 4;
    } else {
        n = 0;
    }

    mpi::GraphCommunicator gph(comm, n, sources, degrees, destinations, weights);

    std::cout << gph;
}