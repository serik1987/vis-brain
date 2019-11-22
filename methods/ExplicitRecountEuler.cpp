//
// Created by serik1987 on 21.11.2019.
//

#include "ExplicitRecountEuler.h"

namespace method{

    equ::Ode::SolutionParameters ExplicitRecountEuler::getSolutionParameters() {
        equ::Ode::SolutionParameters parameters(getIntegrationTime(), 2);
        return parameters;
    }

    void ExplicitRecountEuler::initialize(equ::Ode &ode) {
        ode.copy(2, 0);
    }

    void ExplicitRecountEuler::update(equ::Ode &ode, unsigned long long timestamp) {
        ode.swap(0, 2);
        double t = (double)timestamp;
        ode.calculateDerivative(0, 0, t);
        ode.increment(1, 0, 0);
        ode.calculateDerivative(1, 1, t+1.0);
        ode.increment(2, 0, 0, 0.5);
        ode.increment(2, 2, 1, 0.5);
        ode.update(getIntegrationTime()*(double)timestamp);
    }
}
