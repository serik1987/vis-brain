//
// Created by serik1987 on 21.11.2019.
//

#include "ExplicitEuler.h"

namespace method {

    equ::Ode::SolutionParameters ExplicitEuler::getSolutionParameters() {
        equ::Ode::SolutionParameters parameters(getIntegrationTime(), 1, 1, false);
        return parameters;
    }

    void ExplicitEuler::initialize(equ::Ode &ode) {
        ode.copy(1, 0);
    }

    void ExplicitEuler::update(equ::Ode &ode, unsigned long long timestamp) {
        ode.swap(0, 1); // y_n = y_n+1
        ode.calculateDerivative(0, 0, (double)timestamp); // k_0 = f(t_n, y_n)
        ode.increment(1, 0, 0); // y_n+1 = y_n + k_0
        ode.update(getIntegrationTime()*(double)timestamp);
    }

}
