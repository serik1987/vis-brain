//
// Created by serik1987 on 20.11.2019.
//

#ifndef MPI2_SINGLEODE_H
#define MPI2_SINGLEODE_H

#include "Ode.h"
#include "Processor.h"
#include "../data/LocalMatrix.h"

/* These macros will help to implement OdeTemporalKernel::calculateDerivative methods */
#define SINGLE_ODE_DERIVATIVE(n)    getDerivative(n, derivativeIndex).begin()
#define SINGLE_ODE_OUTPUT(n)        getOutput(n, equationIndex, equationBuffer).begin()
#define SINGLE_ODE_DERIVATIVE_LAST_PIXEL(n) getDerivative(n, derivativeIndex).end()
#define SINGLE_ODE_OUTPUT_LAST_PIXEL(n)     getOutput(n, equationIndex, equationBuffer).end()

namespace equ {

    /**
     * A base class for all processors which job is described by the set of ordinary differential equations
     */
    class SingleOde: virtual public Processor, public Ode {

    protected:
        bool isOutputContiguous() override { return false; }

        /**
         *
         * @return the main equation. The main equation is an equation that is used for generation of the
         * output matrix (or getOutput()). This initial getOutput() may be regenerated by means of update
         * method
         */
        virtual int getMainEquation() = 0;

        /**
         * Makes all initialization routine except construction of output and derivatve matrices.
         * Don't forget to use this method to set initial condition to your ODE to the zero output matrix
         * Setting initial conditions to all other output matrices is the responsibility of the solution method,
         * not yours
         */
        virtual void initializeSingleOde() = 0;

        /**
         * Performs all ODE finalization routines except finalization of output and derivative buffers
         *
         * @param destruct false everywhere except in the destructor
         */
        virtual void finalizeSingleOde(bool destruct = false) = 0;

        void finalizeProcessor(bool destruct = false) noexcept override;


    public:

        void increment(int outputNumber, int inputNumber, int derivativeNumber, double incrementStep = 1.0,
                       BufferType inputBuffer = PublicBuffer, BufferType equationBuffer = PublicBuffer) override;

        void swap(int outputIndex1, int outputIndex2) override;

        void copy(int dest, int source) override;

        void swapDerivative(int derivativeIndex1, int derivativeIndex2) override;

        void copyDerivative(int dest, int source) override;

        void fillBuffer() override;

        double getOutputDiscrepancy(int index1, int index2,
                                    BufferType buffer1 = PublicBuffer, BufferType buffer2 = PublicBuffer) override;

        /**
         * Changes the processor state in such a way as getOutput() function will return a reference to
         * a certain output index. (By default, getOutput() returns the reference to 0th output matrix
         *
         * @param index index of the matrix to output
         */
        void setCurrentOutput(int index);

        virtual void update(double time) = 0;

        typedef std::vector<data::LocalMatrix*> Line;

        typedef struct{
            Line* out;
            Line* der;
        } Cell;

        typedef std::vector<Cell> Buffer;

        /**
         * Constructs the single ODE.
         *
         * @param comm communicator that is used to share operations within the single ODE
         * @param parameters solution parameters (see help on equ::Ode::SolutionParameters for the help
         */
        SingleOde(mpi::Communicator& comm, Ode::SolutionParameters parameters):
            Processor(comm), Ode(parameters) {};

        ~SingleOde() override {
            finalizeProcessor(true);
        }

        /**
         *
         * @return matrix resolution on X
         */
        [[nodiscard]] virtual int getGridX() = 0;

        /**
         *
         * @return matrix resolution on Y
         */
        [[nodiscard]] virtual int getGridY() = 0;

        /**
         *
         * @return matrix size on X in um or any other units
         */
        [[nodiscard]] virtual double getSizeX() = 0;

        /**
         *
         * @return matrix size on Y or any other units
         */
        [[nodiscard]] virtual double getSizeY() = 0;

        /**
         * Initializes the processor
         */
        void initialize() override;

        /**
         * The output is treated as output matrix with index set by setCurrentOutput(...) method,
         * equation with index equal to getMainEquation(), in the PublicBuffer
         *
         * @return the output
         */
        data::Matrix& getOutput() override{
            return *buffers[PublicBuffer]->at(getMainEquation()).out->at(currentOutput);
        }

        /**
         * Returns output of the particular number for a partucular equation
         *
         * @param equationNumber
         * @param outputNumber
         * @return
         */
        data::LocalMatrix& getOutput(int equationNumber, int outputNumber, BufferType type = PublicBuffer){
            return *buffers[type]->at(equationNumber).out->at(outputNumber);
        }

        /**
         * Returns the derivative matrix
         *
         * @param equationNumber number of equation which derivative matrix shall be returned
         * @param matrixNumber index of the derivative matrix
         * @return
         */
        data::LocalMatrix& getDerivative(int equationNumber, int matrixNumber){
            return *buffers[PublicBuffer]->at(equationNumber).der->at(matrixNumber);
        }

    protected:
        Buffer* buffers[2];

    private:
        bool initialized = false;
        int currentOutput = -1;

        Line* initializeLine(int matrixNumber);
        void finalizeLine(Line*);
    };

}


#endif //MPI2_SINGLEODE_H
