//
// Created by serik1987 on 20.11.2019.
//

#ifndef MPI2_ODE_H
#define MPI2_ODE_H


namespace equ{

    /**
     * An interface that is required for the solution method.
     * With this interface this method may solve any equation.
     * Also, this is a base class for equ::State and equ::SingleOde
     *
     * The interface uses the following parameters:
     * output matrices: matrices where different approximations of the output data are stored. Each matrix
     * is denoted by the index.
     *
     * derivative matrices: matrices where different derivatives at different timestamps are stored. Each matrix
     * is denoted by the index.
     *
     * By default, all output and derivative matrices are stored in the public buffer. This is the only buffer
     * if the doubleBuffer parameter is false. However, when doubleBuffer is true, output and derivative data
     * calculated at two consequtive iterations may be stored at two buffers: the public buffer and the private buffer
     */
    class Ode{
    public:

        /**
         * This class contains general information about the solution method
         * An instance of this class is returned by the certain solution method.
         * This instance shall be substituted into the equ::State instance as a parameter.
         * In turn, equ::State will substitute this instance into each equ::SingleOde processor or
         * equ::StateOde processor. total number of equations within the processor will be equal to the
         * equationNumber. All equation parameters shall be taken into consideration during initialization
         * of equ::SingleOde
         *
         * The following method parameters will be returned or set (help on corresponding getters/setters):
         *
         *      getTimeConstant(), setTimeConstant(double): sets the multiplication term before d/dt multiplied
         *      by the integration step. The parameter will be set by the integration method to be equal to the
         *      integration time constant but during the method usage this value will be switched to the
         *      integration step multiplied by the integration time constant
         *
         *      getEquationOrder(), setEquationOrder(int): defines an equation order; total number of output matries
         *      will always be greater than the equation order by one.
         *
         *      getDerivativeOrder(), setDerivativeOrder(int): defines the derivative order; this is the same as
         *      total number of derivatives in the buffer
         *
         *      isDoubleBuffer(), setDoubleBuffer(bool): when true, defines two copies of the buffer: the hidden
         *      buffer and the public buffer. This is very helpful when you apply implicit solution methods
         *      and you need several iterations
         *
         *      getEquationNumber(), setEquationNumber(int): total number of equations in the system. Default number
         *      is 1
         */
        class SolutionParameters{
        private:
            double timeConstant;
            double integrationStep;
            int equationOrder;
            int derivativeOrder;
            bool doubleBuffer;
            int equationNumber = 1;

        public:
            /**
             * Initialize the default solution parameters:
             * time constant: 1.0 sample
             * equation order: 1
             * derivative order: 1
             * double buffer: false
             */
            SolutionParameters(): timeConstant(1.0), equationOrder(1), derivativeOrder(1), doubleBuffer(false),
                integrationStep(1.0){};

            /**
             * Initialize the default solution parameters
             * time constant (in samples): as given in the argument
             * equation order: 1
             * derivative order: 1
             * double buffer: false
             *
             * @param timeConstant
             */
            explicit SolutionParameters(double timeConstant): timeConstant(timeConstant), equationOrder(1),
                derivativeOrder(1), doubleBuffer(false), integrationStep(timeConstant) {};

            /**
             * Initialize the default solution parameters
             * time constant (in samples): as given in the argument
             * equation_order: as given as in the argument
             * derivative order: the same as equation order
             * double buffer: false
             *
             * @param timeConstant
             * @param equationOrder
             */
            SolutionParameters(double timeConstant, int equationOrder): timeConstant(timeConstant),
                equationOrder(equationOrder), derivativeOrder(equationOrder), doubleBuffer(false),
                integrationStep(timeConstant){};

            /**
             * Initializes the solution parameters
             *
             * @param timeConstant
             * @param equationOrder
             * @param derivativeOrder
             * @param doubleBuffer
             */
            SolutionParameters(double timeConstant, int equationOrder, int derivativeOrder, bool doubleBuffer = false):
                timeConstant(timeConstant), equationOrder(equationOrder), derivativeOrder(derivativeOrder),
                doubleBuffer(doubleBuffer), integrationStep(timeConstant) {};

            [[nodiscard]] double getTimeConstant() const { return timeConstant; }
            void setTimeConstant(double value) { timeConstant = value; }

            [[nodiscard]] int getEquationOrder() const { return equationOrder; }
            void setEquationOrder(int order) { equationOrder = order; }

            [[nodiscard]] int getDerivativeOrder() const { return derivativeOrder; }
            void setDerivativeOrder(int order) { derivativeOrder = order; }

            [[nodiscard]] bool isDoubleBuffer() const { return doubleBuffer; }
            void setDoubleBuffer(bool value) { doubleBuffer = value; }

            [[nodiscard]] int getEquationNumber() const { return equationNumber; }
            void setEquationNumber(int value) { equationNumber = value; }

            /**
             *
             * @return the time integration step. The method is set by the integration method. Its change is
             * strictly prohibited.
             */
            [[nodiscard]] double getIntegrationStep() { return integrationStep; }
        };

    private:
        SolutionParameters solutionParameters;

    public:
        /**
         * Initializes the ODE objects
         *
         * @param parameters solution parameters returned by the method
         */
        explicit Ode(SolutionParameters parameters): solutionParameters(parameters) {};

        /**
         * Returns current solution parameters
         *
         * @return reference to the current solution parameters
         */
        [[nodiscard]] SolutionParameters& getSolutionParameters() { return solutionParameters; }

        enum BufferType { PublicBuffer = 0, PrivateBuffer = 1};

    public:

        /**
         * Calculates the derivative of the output. The derivative will be placed into public buffer
         *
         * Note: derivative matrix for n-th equation may be revealed by:
         *
         * @param derivativeIndex index of the derivative matrix where the calculated output derivative shall be
         * placed
         * @param equationIndex index of the output matrix where the data for calculating derivative shall be taken
         * @param t current time, in timestamps. This is equation's responsibility to transform this value from
         * milliseconds to timestamps
         * @param equationBuffer PublicBuffer if the equation shall be taken from the public buffer or private buffer
         * if the equation shall be taken from the private buffer
         */
        virtual void calculateDerivative(int derivativeIndex, int equationIndex,
                double t, BufferType equationBuffer = PublicBuffer) = 0;

        /**
         * Increments the output value according to the calculated derivative. New incremented output will be placed to
         * the public buffer
         *
         * @param outputNumber index of the output matrix where output values shall be stored
         * @param inputNumber index of the input matrix which output values shall be incremented
         * @param derivativeNumber index of the derivative that shall be used for incrementation
         * @param incrementStep step at which the values shall be incremented
         * @param incrementStep step at which the values shall be incremented, in timestamps. Usually holds the value
         * from 0.0 to 1.0, 1.0 corresponds to the timestamp size
         * @param inputBuffer public or private buffer which data shall be incremented
         * @param outputBuffer public or private buffer which derivatives shall be taken
         */
        virtual void increment(int outputNumber, int inputNumber, int derivativeNumber, double incrementStep = 1.0,
                BufferType inputBuffer = PublicBuffer, BufferType equationBuffer = PublicBuffer) = 0;

        /**
         * Swaps two outputs. Arguments are indices of the outputs to swap.
         * The swap will be hold in the public buffer
         *
         * @param outputIndex1
         * @param outputIndex2
         */
        virtual void swap(int outputIndex1, int outputIndex2) = 0;

        /**
         * Copies the output matrix
         *
         * @param dest index of the destination output matrix
         * @param source index of the source output matrix
         */
        virtual void copy(int dest, int source) = 0;

        /**
         * Swaps two derivatives located in the PublicBuffer
         *
         * @param derivativeIndex1
         * @param derivativeIndex2
         */
        virtual void swapDerivative(int derivativeIndex1, int derivativeIndex2) = 0;

        /**
         * Copies the derivative matrix
         *
         * @param dest the destination derivative matrix
         * @param source  the source derivative matrix
         */
        virtual void copyDerivative(int dest, int source) = 0;

        /**
         * Copies all output and derivative matrices from public buffer into the private buffer
         */
        virtual void fillBuffer() = 0;

        /**
         * Returns the maximum discrepancy between the buffer 1 and the buffer 2
         * This routine may be collective is the collective routine is required to get the maximum
         *
         * @param index1 index of the first matrix to compare
         * @param index2 index of the second matrix to compare
         * @param buffer1 public or private buffer for the first output matrix
         * @param buffer2 public of private buffer for the second output matrix
         * @return
         */
        [[nodiscard]] virtual double getOutputDiscrepancy(int index1, int index2,
                BufferType buffer1 = PublicBuffer, BufferType buffer2 = PublicBuffer) = 0;

        /**
         * Performs any computations after the output was updated.
         *
         * @param time
         */
        virtual void update(double time) = 0;
    };

}

#endif //MPI2_ODE_H
