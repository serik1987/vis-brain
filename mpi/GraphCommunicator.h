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
        int getSourceNumber() const { return sourceNumber; }

        /**
         *
         * @return a reference to the first source
         */
        const int* getSourceList() const { return sources; }

        /**
         *
         * @return the number of destination nodes
         */
        int getDestinationNumber() const { return destinationNumber; }

        /**
         *
         * @return a reference to the destination node
         */
        const int* getDestinationList() const { return destinations; }

        /**
         *
         * @return true if weights are given, false otherwise
         */
        bool isWeighted() { return weighted; }

        /**
         * Provides an output to a certain stream
         *
         * @param out a certain stream
         * @param comm communicator
         * @return e reference to out
         */
        friend std::ostream& operator<<(std::ostream& out, GraphCommunicator& comm);

        /**
         * Sends the information from the process i to the process j if and only if (i, j) is an edge of the graph
         *
         * @param sendbuf reference to the information to send. The buffer shall be allocated for sendcount items
         * The same information containing in sendbuf will transmit by all edges in the graph
         * @param sendcount total number of items to send
         * @param sendtype type of sending items
         * @param recvbuf the buffer where receiving information will be placed. The information received from
         * k-th source will be placed in the region starting from recvbuf + i * recvcount. The buffer shall be
         * allocated for recvcount * n items where n is number of sources
         * @param recvcounts total number of items to receive
         * @param recvtype type of receiving items
         */
        void neighborAllGather(const void* sendbuf, int sendcount, MPI_Datatype sendtype,
                void* recvbuf, int recvcounts, MPI_Datatype recvtype){
            int errcode;
            if ((errcode = MPI_Neighbor_allgather(sendbuf, sendcount, sendtype,
                    recvbuf, recvcounts, recvtype, comm)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
        }

        /**
         * The same as neighborAllGather but doesn't block the process
         *
         * @return request. See mpi::Request and mpi::Requests for detail
         */
        MPI_Request ineighborAllGather(const void* sendbuf, int sendcount, MPI_Datatype sendtype,
                               void* recvbuf, int recvcounts, MPI_Datatype recvtype){
            MPI_Request request;
            int errcode;
            if ((errcode = MPI_Ineighbor_allgather(sendbuf, sendcount, sendtype,
                    recvbuf, recvcounts, recvtype, comm, &request)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            return request;
        }


        /**
         * Sends the information from the process i to the process j if and only if (i, j) is an edge of the graph
         *
         * @param sendbuf reference to the information to be send to the destination processes. The same information
         * will be sent to all processes. The buffer shall be allocated for sendcount items
         * @param sendcount total number of items to end
         * @param sendtype datatype of sending items
         * @param recvbuf buffer where all receiving information will be placed. the information received from
         * the kth neighbor will be placed at rhe region starting from recvbuf + displs[k]
         * @param recvcounts[k] total number of items expected to receive from k-th source
         * @param displs[k] index of the first data received from k-th source and placed into
         * @param recvtype type of all receiving items
         */
        void neighborAllGather(const void* sendbuf, int sendcount, MPI_Datatype sendtype,
                void* recvbuf, const int recvcounts[], const int displs[], MPI_Datatype recvtype){
            int errcode;
            if ((errcode = MPI_Allgatherv(sendbuf, sendcount, sendtype,
                    recvbuf, recvcounts, displs, recvtype, comm)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
        }

        /**
         * The same as neighborAllGather but doesn't block the process
         *
         * @return request. See mpi::Request and mpi::Requests for detail
         */
        MPI_Request ineighborAllGather(const void* sendbuf, int sendcount, MPI_Datatype sendtype,
                               void* recvbuf, const int recvcounts[], const int displs[], MPI_Datatype recvtype){
            MPI_Request request;
            int errcode;
            if ((errcode = MPI_Ineighbor_allgatherv(sendbuf, sendcount, sendtype,
                    recvbuf,recvcounts, displs, recvtype, comm, &request)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            return request;
        }

        /**
         * Sends the information from the process i to the process j only if (, j) is an edge of the graph
         *
         * @param sendbuf buffer contasining the sending information. The information to send to the destination
         * number k is located at a region starting from sendbuf + sendcount * k
         * @param sendcount total number of items to send. The same number of items will send to all destinations
         * @param sendtype type of the data to send
         * @param recvbuf buffer where all receiving items will be placed. The information to receive from
         * the source number k will be put to a region starting from recvbuf + recvcount * k
         * @param recvcount total number of items to receive. The same numnber of items will be received from
         * all sources
         * @param recvtype datatype of the receiving data
         */
        void neighborAllToAll(const void* sendbuf, int sendcount, MPI_Datatype sendtype,
                void* recvbuf, int recvcount, MPI_Datatype recvtype){
            int errcode;
            if ((errcode = MPI_Neighbor_alltoall(sendbuf, sendcount, sendtype,
                    recvbuf,recvcount, recvtype, comm)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
        }

        /**
         * The same as neighborAllToAll but doesn't block the process
         *
         * @return request. See mpi::Request and mpi::Requests for detail
         */
        MPI_Request ineighborAllToAll(const void* sendbuf, int sendcount, MPI_Datatype sendtype,
                              void* recvbuf, int recvcount, MPI_Datatype recvtype){
            MPI_Request request;
            int errcode;
            if ((errcode = MPI_Ineighbor_alltoall(sendbuf, sendcount, sendtype,
                    recvbuf, recvcount, recvtype, comm, &request)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            return request;
        }

        /**
         * Sends the information from the process i to the process j only if (, j) is an edge of the graph
         *
         * @param sendbuf buffer containing all data to send. The data to send to k-th destination is located at a
         * region starting from sendbuf + sdispls[k]
         * @param sendcounts[k] number of items to send to k-th destination
         * @param sdispls see above
         * @param sendtype type of the sending data
         * @param recvbuf buffer where all received items will be put. The data received from k-th source will
         * be put on region starting from recvbuf + rdispls[k]
         * @param recvcounts[k] total number of items expected to receive from k-th source
         * @param rdispls[k] (see above)
         * @param recvtype type of the receiving data
         */
        void neighborAllToAll(const void* sendbuf, const int sendcounts[], const int sdispls[], MPI_Datatype sendtype,
                void* recvbuf, const int recvcounts[], const int rdispls[], MPI_Datatype recvtype){
            int errcode;
            if ((errcode = MPI_Neighbor_alltoallv(sendbuf, sendcounts, sdispls, sendtype,
                    recvbuf, recvcounts, rdispls, recvtype, comm)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
        }

        /**
         * The same as neighborAllToAll but doesn't block the process
         *
         * @return request. See mpi::Request and mpi::Requests for detail
         */
        MPI_Request ineighborAllToAll(const void* sendbuf, const int sendcounts[], const int sdispls[], MPI_Datatype sendtype,
                              void* recvbuf, const int recvcounts[], const int rdispls[], MPI_Datatype recvtype){
            MPI_Request request;
            int errcode;
            if ((errcode = MPI_Ineighbor_alltoallv(sendbuf, sendcounts, sdispls, sendtype,
                    recvbuf, recvcounts, rdispls, recvtype, comm, &request)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            return request;
        }

        /**
         * Sends the information from the process i to the process j only if (, j) is an edge of the graph
         *
         * @param sendbuf buffer containing all data to send. The data to send to k-th destination is located at a
         * region starting from sendbuf + sdispls[k]
         * @param sendcounts[k] number of items to send to k-th destination
         * @param sdispls see above
         * @param sendtypes[k] type of the data to send to k-th destination
         * @param recvbuf buffer where all received items will be put. The data received from k-th source will
         * be put on region starting from recvbuf + rdispls[k]
         * @param recvcounts[k] total number of items expected to receive from k-th source
         * @param rdispls[k] (see above)
         * @param recvtype[k] type of the data to receive from k-th source
         */
        void neighborAllToAll(
                const void* sendbuf, const int sendcounts[], const MPI_Aint sdispls[], const MPI_Datatype sendtypes[],
                void* recvbuf, const int recvcounts[], const MPI_Aint rdispls[], const MPI_Datatype recvtypes[]){
            int errcode;
            if ((errcode = MPI_Neighbor_alltoallw(sendbuf, sendcounts,sdispls, sendtypes,
                    recvbuf, recvcounts, rdispls, recvtypes, comm)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
        }

        /**
         * The same as neighborAllToAll but doesn't block the process
         *
         * @return request. See mpi::Request and mpi::Requests for detail
         */
        MPI_Request ineighborAllToAll(
                const void* sendbuf, const int sendcounts[], const MPI_Aint sdispls[], const MPI_Datatype sendtypes[],
                void* recvbuf, const int recvcounts[], const MPI_Aint rdispls[], const MPI_Datatype recvtypes[]){
            int errcode;
            MPI_Request request;
            if ((errcode = MPI_Ineighbor_alltoallw(sendbuf, sendcounts, sdispls, sendtypes,
                    recvbuf, recvcounts, rdispls, recvtypes,comm, &request)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            return request;
        }
    };

}


#endif //MPI2_GRAPHCOMMUNICATOR_H
