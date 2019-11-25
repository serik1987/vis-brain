//
// Created by serik1987 on 24.11.2019.
//

#ifndef MPI2_STATE_H
#define MPI2_STATE_H

#include <list>
#include "Ode.h"
#include "SingleOde.h"
#include "Processor.h"

namespace equ {

    class State: private std::list<Processor*>, public Ode {
    private:
        mpi::Communicator& comm;
        std::list<SingleOde*> diffList;

        struct ProcessorStackItem {
            Processor* processor = nullptr;
            std::list<Processor*>::iterator position;
        };

    public:
        explicit State(mpi::Communicator& comm, SolutionParameters parameters):
            comm(comm), list<Processor*>(), Ode(parameters) {};

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
        void calculateDerivative(int derivativeIndex, int equationIndex,
                                 double t, BufferType equationBuffer = PublicBuffer) override;

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
        void increment(int outputNumber, int inputNumber, int derivativeNumber, double incrementStep = 1.0,
                       BufferType inputBuffer = PublicBuffer, BufferType equationBuffer = PublicBuffer) override;

        /**
         * Swaps two outputs. Arguments are indices of the outputs to swap.
         * The swap will be hold in the public buffer
         *
         * @param outputIndex1
         * @param outputIndex2
         */
        void swap(int outputIndex1, int outputIndex2) override;

        /**
         * Copies the output matrix
         *
         * @param dest index of the destination output matrix
         * @param source index of the source output matrix
         */
        void copy(int dest, int source) override;

        /**
         * Swaps two derivatives located in the PublicBuffer
         *
         * @param derivativeIndex1
         * @param derivativeIndex2
         */
        void swapDerivative(int derivativeIndex1, int derivativeIndex2) override;

        /**
         * Copies the derivative matrix
         *
         * @param dest the destination derivative matrix
         * @param source  the source derivative matrix
         */
        void copyDerivative(int dest, int source) override;

        /**
         * Copies all output and derivative matrices from public buffer into the private buffer
         */
        void fillBuffer() override;

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
        [[nodiscard]] double getOutputDiscrepancy(int index1, int index2,
                                                          BufferType buffer1 = PublicBuffer,
                                                          BufferType buffer2 = PublicBuffer) override;

        /**
         *
         * @param time
         */
        void update(double time) override;

        /**
         * Destroys all processors within the list
         * Please, don't destroy the processes after the state destruction
         */
        virtual ~State();


        Processor* createProcessor(const std::string& mechanism){
            return Processor::createProcessor(comm, mechanism, getSolutionParameters());
        };

        /**
         * Adds processor with all child processors
         *
         * @param proc pointer to the processor
         */
        void addProcessor(Processor* proc);

#if DEBUG==1
        /**
         * Prints all processors from the list
         */
        void printProcessorList();
#endif

        /**
         * Initializes all processes within the state
         */
        void initialize();

        /**
         * Finalizes all processes within the state
         */
        void finalize();

    };

}


#endif //MPI2_STATE_H
