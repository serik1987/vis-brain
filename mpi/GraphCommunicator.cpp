//
// Created by User on 10.07.2019.
//

#include "GraphCommunicator.h"

void mpi::GraphCommunicator::receiveGraphInfo() {
    int errcode;
    int work;
    if ((errcode = MPI_Dist_graph_neighbors_count(comm, &sourceNumber, &destinationNumber, &work)) != MPI_SUCCESS){
        throw_exception(errcode);
    }
    weighted = work != 0;
    sources = new int[sourceNumber];
    destinations = new int[destinationNumber];
    sweights = new int[sourceNumber];
    dweights = new int[destinationNumber];
    if ((errcode = MPI_Dist_graph_neighbors(comm, sourceNumber, sources, sweights,
            destinationNumber, destinations, dweights)) != MPI_SUCCESS){
        throw_exception(errcode);
    }
}

mpi::GraphCommunicator::GraphCommunicator(mpi::GraphCommunicator &&other): Communicator(std::move(other)) {
    sources = other.sources;
    other.sources = nullptr;
    destinations = other.destinations;
    other.destinations = nullptr;
    sweights = other.sweights;
    other.sweights = nullptr;
    dweights = other.dweights;
    other.dweights = nullptr;
    sourceNumber = other.sourceNumber;
    other.sourceNumber = 0;
    destinationNumber = other.destinationNumber;
    other.destinationNumber = 0;
    weighted = other.weighted;
    other.weighted = false;
}

mpi::GraphCommunicator& mpi::GraphCommunicator::operator=(mpi::GraphCommunicator&& other){
    sources = other.sources;
    other.sources = nullptr;
    destinations = other.destinations;
    other.destinations = nullptr;
    sweights = other.sweights;
    other.sweights = nullptr;
    dweights = other.dweights;
    other.dweights = nullptr;
    sourceNumber = other.sourceNumber;
    other.sourceNumber = 0;
    destinationNumber = other.destinationNumber;
    other.destinationNumber = 0;
    weighted = other.weighted;
    other.weighted = false;
    return *this;
}

mpi::GraphCommunicator::~GraphCommunicator() {
    delete [] sources;
    delete [] destinations;
    delete [] sweights;
    delete [] dweights;
}

namespace mpi {
    std::ostream &operator<<(std::ostream &out, mpi::GraphCommunicator &comm) {
        out << "RANK = " << comm.getRank() << " SOURCES = ";
        for (int i = 0; i < comm.sourceNumber; i++) {
            out << comm.sources[i] << " ";
        }
        out << "DESTINATIONS = ";
        for (int i = 0; i < comm.destinationNumber; i++) {
            out << comm.destinations[i] << " ";
        }
        out << "\n";
        return out;
    }
}