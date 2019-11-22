//
// Created by serik1987 on 21.11.2019.
//

#ifndef MPI2_EXPLICITRECOUNTEULER_H
#define MPI2_EXPLICITRECOUNTEULER_H

#include "Method.h"

namespace method {

    class ExplicitRecountEuler: public Method {
    public:
        /**
         * Creates new method
         *
         * @param dt integration step in ms
         */
        explicit ExplicitRecountEuler(double dt): Method(dt) {};

        [[nodiscard]] equ::Ode::SolutionParameters getSolutionParameters() override;
        void initialize(equ::Ode& ode) override;
        void update(equ::Ode& ode, unsigned long long timestamp) override;
    };

}


#endif //MPI2_EXPLICITRECOUNTEULER_H
