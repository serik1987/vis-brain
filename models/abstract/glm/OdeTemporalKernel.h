//
// Created by serik1987 on 20.11.2019.
//

#ifndef MPI2_ODETEMPORALKERNEL_H
#define MPI2_ODETEMPORALKERNEL_H

#include "TemporalKernel.h"
#include "../../../processors/SingleOde.h"


namespace equ {

    /**
     * A base class for any temporal kernel that is described by the single ODE:
     * tau * dm/dt = U - m
     * tau * dU/dt = I - U, I is output signal from the stimulus saturation
     */
    class OdeTemporalKernel: public SingleOde, public TemporalKernel {
    private:
        double timeConstant = -1.0;
        double initialStimulusValue = -1.0;

    protected:
        std::string getProcessorName() override { return "equ::OdeTemporalKernel"; }
        void loadParameterList(const param::Object& source) override;
        void broadcastParameterList() override;
        void setParameter(const std::string& name, const void* pvalue) override;
        void initializeSingleOde() override;
        void finalizeSingleOde(bool destruct = false) override {};

        int getMainEquation() { return 1; }

    public:
        OdeTemporalKernel(mpi::Communicator& comm, Ode::SolutionParameters parameters):
            SingleOde(comm, parameters), TemporalKernel(comm), Processor(comm) {
            getSolutionParameters().setEquationNumber(2);
        };

        ~OdeTemporalKernel() {
            finalizeSingleOde(true);
        }

        class negative_time_constant: public simulation_exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "Time constant is negative or zero";
            }
        };

        void update(double time) override;

        int getGridX() override {
            return getStimulusSaturation()->getOutput().getWidth();
        }

        int getGridY() override {
            return getStimulusSaturation()->getOutput().getHeight();
        }

        double getSizeX() override{
            return getStimulusSaturation()->getOutput().getWidthUm();
        }

        double getSizeY() override {
            return getStimulusSaturation()->getOutput().getHeightUm();
        }

        void calculateDerivative(int derivativeIndex, int equationIndex,
                                 double t, BufferType equationBuffer = PublicBuffer) override;

        /**
         *
         * @return the time constant in ms
         */
        [[nodiscard]] double getTimeConstant() const {
            return timeConstant;
        }

        /**
         *
         * @return stimulus value at t = 0
         */
        [[nodiscard]] double getInitialStimulusValue() const {
            return initialStimulusValue;
        }

        /**
         * Sets new value of the time contant.
         *
         * @param value time constant in ms
         */
        void setTimeConstant(double value) {
            timeConstant = value;
            double tau = timeConstant;
            double h = getSolutionParameters().getIntegrationStep();
            getSolutionParameters().setTimeConstant(tau/h);
        }

        /**
         * Sets the initial stimulus value.
         * This value is used for setting initial conditions
         *
         * @param value
         */
        void setInitialStimulusValue(double value) { initialStimulusValue = value; }
    };

}


#endif //MPI2_ODETEMPORALKERNEL_H
