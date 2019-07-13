//
// Created by User on 02.07.2019.
//

#include "Communicator.h"
#include "CartesianCommunicator.h"


namespace mpi {

    Communicator Communicator::exclude(int start, int stop, int step) const {
        if (comm == MPI_COMM_NULL) throw communicator_error();
        Group g0(this);
        Group g1 = g0.exclude(start, stop, step);
        return g1.createCommunicator();
    }

    Communicator Communicator::include(int start, int stop, int step) const{
        if (comm == MPI_COMM_NULL) throw communicator_error();
        Group g0(this);
        Group g1 = g0.include(start, stop, step);
        return g1.createCommunicator();
    }

    CartesianCommunicator Communicator::createCartesianTopology(int ndims, const int dims[], const bool periods[], bool reorder) const{
        if (comm == MPI_COMM_NULL) throw communicator_error();
        MPI_Comm comm_cart;
        int errcode;
        std::vector<int> periods_ad(ndims);
        std::copy(periods, periods+ndims, periods_ad.begin());
        if ((errcode = MPI_Cart_create(comm, ndims, dims, &periods_ad[0], (int)reorder, &comm_cart)) != MPI_SUCCESS){
            throw_exception(errcode);
        }
        return CartesianCommunicator(comm_cart, ndims, dims, periods);
    }

}