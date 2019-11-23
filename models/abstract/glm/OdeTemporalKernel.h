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
        double lateTimeConstant = -1.0;
        double lateTimeConstantH = -1.0;
        double initialStimulusValue = -1.0;
        double K = -1.0;

        static constexpr int EQUATION_U = 0;
        static constexpr int EQUATION_m = 1;
        static constexpr int EQUATION_U_LATE = 2;
        static constexpr int EQUATION_m_LATE = 3;

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
            getSolutionParameters().setEquationNumber(4);
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

        class negative_K_error: public simulation_exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "K value can't be negative";
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
         * @return the time constant for an ascending process in ms
         */
        [[nodiscard]] double getTimeConstant() const {
            return timeConstant;
        }

        /**
         *
         * @return time constant for descending process in ms
         */
        [[nodiscard]] double getLateTimeConstant() const {
            return lateTimeConstant;
        }

        /**
         *
         * @return ratio of ascending to descending value
         */
        [[nodiscard]] double getK() const {
            return K;
        }

        /**
         *
         * @return stimulus value at t = 0
         */
        [[nodiscard]] double getInitialStimulusValue() const {
            return initialStimulusValue;
        }

        /**
         * Sets new value of the ascending time constant
         *
         * @param value time constant in ms
         */
        void setTimeConstant(double value) {
            if (value <=  0.0){
                throw negative_time_constant();
            }
            timeConstant = value;
            double tau = timeConstant;
            double h = getSolutionParameters().getIntegrationStep();
            getSolutionParameters().setTimeConstant(tau/h);
        }

        /**
         * Sets new value of the descending time contant.
         *
         * @param value time constant in ms
         */
        void setLateTimeConstant(double value) {
            if (value <= 0.0){
                throw negative_time_constant();
            }
            lateTimeConstant = value;
            double tau = lateTimeConstant;
            double h = getSolutionParameters().getIntegrationStep();
            lateTimeConstantH = tau/h;
        }

        void setK(double value) {
            if (value < 0){
                throw negative_K_error();
            }
            K = value;
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
