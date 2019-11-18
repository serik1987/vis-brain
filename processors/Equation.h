//
// Created by serik1987 on 11.11.2019.
//

#ifndef MPI2_EQUATION_H
#define MPI2_EQUATION_H

#include "Processor.h"

namespace equ {

    /**
     * A base class for all algebraic equations. An algebraic equation
     * characterises the dependency of the output matrix on the set of input matrices
     * that depends on time but doesn't depend on a certain state
     */
    class Equation: public Processor {
    public:
        explicit Equation(mpi::Communicator& comm): Processor(comm) {};

    };

}


#endif //MPI2_EQUATION_H
