//
// Created by User on 10.07.2019.
//

#ifndef MPI2_GRAPHCOMMUNICATOR_H
#define MPI2_GRAPHCOMMUNICATOR_H

#include <iostream>

#include "mpi.h"
#include "Communicator.h"

namespace mpi {

    /**
     * Represents the communicator where all processes are connected to each other according to the predefined graph
     */
    class GraphCommunicator : public Communicator {
    private:
        int *sources = nullptr;
        int *destinations = nullptr;
        int* sweights = nullptr;
        int* dweights = nullptr;
        int sourceNumber;
        int destinationNumber;
        bool weighted;

        void receiveGraphInfo();
    public:

        /**
         * The first way to create communicator: write sources and destinations for each rank
         *
         * @param comm the old communicator
         * @param indegree number of processes that connect to the calling process
         * @param sources list of all processes that connect the calling process
         * @param sourceweights weights of all processes that connect the calling process
         * @param outdegree number of all processes the connecting process connects to
         * @param destinations ranks of all processes the calling process connects to
         * @param destweights weights of all output connections
         * @param reorder (optional) true (default) if reordering shall make for optimization, false otherwise
         * * @param info (optional) additional information for a certain MPI implementation
         */
        GraphCommunicator(const Communicator& old_comm, int indegree, const int sources[], const int sourceweights[],
                int outdegree, const int destinations[], const int destweights[],
                bool reorder = true, MPI_Info info = MPI_INFO_NULL)
                : Communicator(MPI_COMM_NULL){
            int errcode;
            if ((errcode = MPI_Dist_graph_create_adjacent(*old_comm, indegree, sources, sourceweights,
                    outdegree, destinations, destweights, info, reorder, &comm)) != MPI_SUCCESS) {
                throw_exception(errcode);
            }
            receiveGraphInfo();
            deletable = true;
        }

        /**
         * Another constructor that uses the interface from MPI_Neighbor_graph_create
         *
         * @param old_comm the old communicator which processes will include into nodes of this graph Different
         * processes within this communicator shall define different edges of the graph. On the other hand, any calling
         * process may define any edges that are not necessarily belong to the process.
         * @param n Total number of sources which destinations are defined by the calling process.
         * @param sources List of all sources. Shall have a size equal to n.
         * @param degrees array of size n. Defines total number of edges per each source that are defined by the calling
         * process
         * @param destinations array of size degrees[0] + ... + degrees[n-1]. Defines the targets of the edges
         * @param weights array of size degrees[0] + ... + degrees[n-1]. Defines the weights of the edges
         * @param reorder (optional, true by default) true if the processes shall change their rank, false to preserse
         * the ranks
         * @param info (optional) additional information for a certain MPI implementation
         */
        GraphCommunicator(const Communicator& old_comm, int n, const int sources[], const int degrees[],
                const int destinations[], const int weights[], bool reorder = true, MPI_Info info = MPI_INFO_NULL)
                :Communicator(MPI_COMM_NULL){
            int errcode;
            if ((errcode = MPI_Dist_graph_create(*old_comm, n, sources, degrees, destinations, weights,
                    info, reorder, &comm)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            receiveGraphInfo();
            deletable = true;
        }

        GraphCommunicator(GraphCommunicator&& other);

        virtual GraphCommunicator& operator=(GraphCommunicator&& other);

        ~GraphCommunicator();

        /**
         *
         * @return the number of source node
         */
        int getSourceNumber(){ return sourceNumber; }

        /**
         *
         * @return the number of destination nodes
         */
        int getDestinationNumber() { return destinationNumber; }

        /**
         *
         * @return true if weights are given, false otherwise
         */
        bool isWeighted() { return weighted; }

        friend std::ostream& operator<<(std::ostream& out, GraphCommunicator& comm);

        /*
        void neighborAllGather(const void* sendbuf, int sendcount, MPI_Datatype sendtype,
                void* recvbuf, int recvcounts, MPI_Datatype recvtype){
            int errcode;
            if ((errcode = MPI_Neighbor(sendbuf, sendcount, sendtype,
                    recvbuf, recvcounts, recvtype, comm)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
        }
         */
    };

}


#endif //MPI2_GRAPHCOMMUNICATOR_H
