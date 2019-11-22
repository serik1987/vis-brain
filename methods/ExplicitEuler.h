//
// Created by serik1987 on 21.11.2019.
//

#ifndef MPI2_EXPLICITEULER_H
#define MPI2_EXPLICITEULER_H

#include "Method.h"

namespace method {

    class ExplicitEuler: public Method {
    public:
        explicit ExplicitEuler(double ts = 1.0): Method(ts) {};

        equ::Ode::SolutionParameters getSolutionParameters() override;
        void initialize(equ::Ode& ode) override;
        void update(equ::Ode& ode, unsigned long long timestamp) override;
    };

}


#endif //MPI2_EXPLICITEULER_H
