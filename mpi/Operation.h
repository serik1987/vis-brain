//
// Created by User on 09.07.2019.
//

#ifndef MPI2_OPERATION_H
#define MPI2_OPERATION_H

#include "mpi.h"

namespace mpi {

    /**
     * Represents the operation for such functions as Communicator::reduce, Communicator::reduceAll,
     * communicator::reduceScatter and so on.
     *
     * These function require an instance of MPI_Op
     * The type transformation from Operation to MPI_Op is defined and implicit
     * MPI_Op can also be easily transformed to Operation.
     *
     * List of predefined operations:
     * MPI_SUM - +
     * MPI_MAX
     * MPI_MIN
     * MPI_PROD - *
     * MPI_LAND - &&
     * MPI_LOR - ||
     * MPI_LXOR - ^^
     * MPI_BAND - &
     * MPI_BOR - |
     * MPI_BXOR - ^
     */
    class Operation {
    private:
        int commute = -1;
        bool commuteComputed = false;
    protected:
        MPI_Op op;
    public:

        Operation(): op(MPI_OP_NULL) {};
        Operation(MPI_Op other): op(other) {};
        virtual ~Operation() {};

        /**
         * Performs type conversion during the assignment
         *
         * @param other - the data to be assigned
         * @return *this
         */
        Operation& operator=(const MPI_Op& other){
            op = other;
            return *this;
        }

        /**
         * Transformation function
         *
         * @return an instance of MPI_Op
         */
        operator MPI_Op(){
            return op;
        }

        bool isCommute(){
            if (!commuteComputed){
                int errcode;
                if ((errcode = MPI_Op_commutative(op, &commute)) != MPI_SUCCESS){
                    throw_exception(errcode);
                }
                commuteComputed = true;
            }
            return commute != 0;
        }
    };

    /**
     * Operation represented by an arbitrary function. Please, bear in mind that such operations are prohibited to copy
     *
     */
     class ComplexOperation: public Operation{
     public:
         ComplexOperation() = delete;
         ComplexOperation(MPI_Op other) = delete;

         /**
          * A standard way to create the complex operation
          *
          * @param func - the function defined in the following way:
          * typedef void (MPI_User_function) ( void * a,
               void * b, int * len, MPI_Datatype * );
               where the operation is b[i] = a[i] op b[i], for i=0,...,len-1.
          * A pointer to the datatype given to the MPI collective computation routine
          * (i.e., MPI_Reduce, MPI_Allreduce, MPI_Scan, or MPI_Reduce_scatter) is also
          * passed to the user-specified routine.s
          * @param commute - true if the operation is commutable (a op b = b op a
          */
         ComplexOperation(MPI_User_function* func, bool commute): Operation(){
             int c = commute;
             int errcode;
             if ((errcode = MPI_Op_create(func, c, &op)) != MPI_SUCCESS){
                 throw_exception(errcode);
             }
         }

         ~ComplexOperation(){
             if (op != MPI_OP_NULL){
                 int errcode;
                 if ((errcode = MPI_Op_free(&op)) != MPI_SUCCESS){
                     throw_exception(errcode);
                 }
             }
         }

         ComplexOperation(const ComplexOperation& other) = delete;
         ComplexOperation(ComplexOperation&& other){
             op = other.op;
             other.op = MPI_OP_NULL;
         }

         Operation& operator=(const MPI_Op& op) = delete;
         Operation& operator=(const Operation& other) = delete;

     };

}

#endif //MPI2_OPERATION_H
