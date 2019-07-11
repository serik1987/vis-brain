//
// Created by User on 02.07.2019.
//

#ifndef MPI2_CARTESIANCOMMUNICATOR_H
#define MPI2_CARTESIANCOMMUNICATOR_H

#include <vector>
#include "Communicator.h"

namespace mpi {

    /**
     * A communicator with a special cartesian topology. Within this communicator the data transmission is
     * possible to so called 'neighbor processes' only. The network of the neighbor process looks like 1D
     * or 2D or 3D grid. The topology between the process looks like this:
     *
     * Process # 0 <-> Process # 1 <-> Process # 2
     * ^                   ^               ^
     * |                   |               |
     * v                   v               v
     * Process # 3 <-> Process # 4 <-> Process # 5
     * ^                   ^               ^
     * |                   |               |
     * v                   v               v
     * Process # 6 <-> Process # 7 <-> Process # 8
     */
    class CartesianCommunicator: public Communicator {

    std::vector<int> dims;
    std::vector<bool> periods;
    std::vector<int> coords;

    public:

        /**
         * Represents the info about the neighbor
         *
         * source - rank of the source process
         * destination - rank of the destination process
         */
        struct NeighborInfo{
            int source;
            int destination;
        };

        /**
         * Represents the information flow
         *
         * upward - from the process with lower rank to the process with higher rank
         * downward - from the process with higher rank to the process with lower rank
         */
        enum Displacement {upward = 1, downward = -1};


        /**
         * Creates an 'empty' cartesian communicator. The function shall not be launched separately.
         * Use Communicator::createCartesianTopology for such purpose, please, don't use this constructor
         *
         * @param c - the communicator
         * @param ndims - number of dimensions
         * @param dims_list - list of all dimensions
         * @param periods_[j] - true if j-th dimension is periodic, false otherwise
         */
        CartesianCommunicator(MPI_Comm c, int ndims, const int dims_list[], const bool periods_[]): Communicator(c),
        dims(dims_list, dims_list+ndims), periods(periods_, periods_+ndims), coords(){
            deletable = false;
        }


        /**
         * Retutns the 'coordinates' of the process with a certain rank
         *
         * @param coords - an array where list of coordinates will be written. The array shall be allocated for
         * elements which number is the same as the number of dimensions in this communicator
         * @param rank - rank of the process which coordinates you want to know (optional, default - rank of the
         * process that called this method
         * @param maxdims - optional argument
         */
        void getCoordinates(int coords[], int rank = -1, int maxdims = -1){
            if (rank == -1) rank = getRank();
            if (maxdims == -1) maxdims = (int)dims.size();
            int errcode;
            if ((errcode = MPI_Cart_coords(comm, rank, maxdims, coords)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
        }

        /**
         * Returns the coordinates of the process that called this routine as std::vector<int> instance
         *
         * @return the vector containing all coordinates of the calling process
         */
        const std::vector<int>& getCoordinates(){
            if (coords.size() == 0){
                coords = std::vector<int>(dims.size());
                getCoordinates(&coords[0]);
            }
            return coords;
        }

        /**
         * Returns the grid size
         *
         * @return a vector. Its j-th item represents the size of j-th dimension of the grid
         */
        const std::vector<int>& getDimensions() const{
            return dims;
        }

        /**
         * Returns whether dimensions are periodic
         *
         * @return a vector. Its j-th dimention represents whether j-th dimension of the grid is periodic
         */
        const std::vector<bool> getPeriods() const{
            return periods;
        }


        /**
         * Returns the information about the neighbor process
         *
         * @param dim - dimension along which you want to test the neighbors
         * @param dest - the general direction of the information flow
         * @return the structure mpi::CartesianCommunicator::NeighborInfo. See help on this structure for details
         */
        NeighborInfo getNeighbors(int dim, Displacement dest) const{
            int errcode;
            NeighborInfo info;
            if ((errcode = MPI_Cart_shift(comm, dim, (int)dest, &info.source, &info.destination)) != MPI_SUCCESS){
                throw_exception(errcode);
            }
            return info;
        }

    };

}


#endif //MPI2_CARTESIANCOMMUNICATOR_H
