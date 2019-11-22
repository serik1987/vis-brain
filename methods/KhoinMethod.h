//
// Created by serik1987 on 21.11.2019.
//

#ifndef MPI2_KHOINMETHOD_H
#define MPI2_KHOINMETHOD_H

#include "Method.h"

namespace method {

    class KhoinMethod: public Method {
    public:
        explicit KhoinMethod(double dt): Method(dt) {};

        [[nodiscard]] equ::Ode::SolutionParameters getSolutionParameters() override;
        void initialize(equ::Ode& ode) override;
        void update(equ::Ode& ode, unsigned long long timestamp) override;
    };

}


#endif //MPI2_KHOINMETHOD_H
