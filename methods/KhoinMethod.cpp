//
// Created by serik1987 on 21.11.2019.
//

#include "KhoinMethod.h"

namespace method{

    equ::Ode::SolutionParameters KhoinMethod::getSolutionParameters() {
        equ::Ode::SolutionParameters parameters(getIntegrationTime(), 3);
        return parameters;
    }

    void KhoinMethod::initialize(equ::Ode &ode) {
        ode.copy(3, 0);
    }

    void KhoinMethod::update(equ::Ode &ode, unsigned long long timestamp) {
        auto t = (double)timestamp;
        auto real_t = t * getIntegrationTime();
        ode.swap(0, 3);
        ode.calculateDerivative(0, 0, t);
        ode.increment(1, 0, 0, 1.0/3);
        ode.calculateDerivative(1, 1, t+1.0/3);
        ode.increment(2, 0, 1, 2.0/3);
        ode.calculateDerivative(2, 2, t+2.0/3);
        ode.increment(3, 0, 0, 0.25);
        ode.increment(3, 3, 2, 0.75);
        ode.update(real_t);
    }
}