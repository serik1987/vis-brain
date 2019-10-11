#include "mpi.h"
#include "../mpi/App.h"
#include "../mpi/GraphCommunicator.h"

int main(int argc, char* argv[]){
    mpi::App app(&argc, &argv);
    mpi::Communicator& comm = app.getAppCommunicator();
    int numprocs = comm.getProcessorNumber();
    int rank = comm.getRank();

    int indegree;
    int sources[10];
    int sweights[10];
    int outdegree;
    int destinations[10];
    int weights[10];

    switch (rank){
        case 0:
            indegree = 1;
            sources[0] = 4;         sweights[0] = 1;
            outdegree = 3;
            destinations[0] = 3;    weights[0] = 1;
            destinations[1] = 4;    weights[1] = 1;
            destinations[2] = 1;    weights[2] = 10;
            break;
        case 1:
            indegree = 2;
            sources[0] = 0;         sweights[0] = 10;
            sources[1] = 2;         sweights[1] = 1;
            outdegree = 0;
            break;
        case 2:
            indegree = 1;
            sources[0] = 3;         sweights[0] = 10;
            outdegree = 1;
            destinations[0] = 1;    weights[0] = 1;
            break;
        case 3:
            indegree = 1;
            sources[0] = 0;         sweights[0] = 1;
            outdegree = 2;
            destinations[0] = 2;    weights[0] = 10;
            destinations[1] = 5;    weights[1] = 1;
            break;
        case 4:
            indegree = 2;
            sources[0] = 5;         sweights[0] = 10;
            sources[1] = 0;         sweights[1] = 1;
            outdegree = 1;
            destinations[0] = 0;    weights[0] = 1;
            break;
        case 5:
            indegree = 1;
            sources[0] = 3;         sweights[0] = 1;
            outdegree = 1;
            destinations[0] = 4;    weights[0] = 10;
            break;
        default:
            indegree = 0;
            outdegree = 0;
    }

    app << "Sources = ";
    for (int i=0; i < indegree; i++){
        app << sources[i] << " ";
    }
    app << " Destinations = ";
    for (int i=0; i < outdegree; i++){
        app << destinations[i] << " ";
    }
    app << "\n";

    mpi::GraphCommunicator gcomm(comm, indegree, sources, sweights, outdegree, destinations, weights);

    app << "Application rank: " << gcomm.getRank() << "\n";
    app << "Total number of source nodes: " << gcomm.getSourceNumber() << "\n";
    app << "Total number of destination nodes: " << gcomm.getDestinationNumber() << "\n";
    app << "Edges are weighted: " << gcomm.isWeighted() << "\n";

    std::cout << gcomm;
}