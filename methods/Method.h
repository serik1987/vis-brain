//
// Created by serik1987 on 21.11.2019.
//

#ifndef MPI2_METHOD_H
#define MPI2_METHOD_H

#include <cmath>
#include "../processors/Ode.h"

namespace method{

    /**
     * Base class for all solution methods
     */
    class Method{
    private:
        double integrationTime;

    public:
        /**
         * Initializes the integration method
         *
         * @param h integration step in ms
         */
        explicit Method(const double h = 1.0): integrationTime(h) {};

        virtual ~Method() {};

        /**
         *
         * @return integration step in ms
         */
        [[nodiscard]] double getIntegrationTime() const { return integrationTime; }

        /**
         * Sets the integration time
         *
         * @param value integration step in ms
         */
        void setIntegrationTime(double value) { integrationTime = value; }

        /**
         * Generates the solution parameters for a certain method. These solution parameters shall be used
         * for creating any single ODE in the network.
         *
         * @return solution parameters.
         */
        virtual equ::Ode::SolutionParameters getSolutionParameters() = 0;

        /**
         * Initializes the ODE
         *
         * @param ode reference to the ODE to be initialized
         */
        virtual void initialize(equ::Ode& ode) = 0;

        /**
         * Advances the equation
         *
         * @param ode equation to advance
         * @param timestamp current timestamp
         */
        virtual void update(equ::Ode& ode, unsigned long long timestamp) = 0;

        /**
         * Advances the equation
         *
         * @param ode equation to advance
         * @param time current time in ms
         */
        virtual void update(equ::Ode& ode, double time){
            auto ts = (unsigned long long)round(time/getIntegrationTime());
            update(ode, ts);
        }
    };

}

#endif //MPI2_METHOD_H
