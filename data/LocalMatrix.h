//
// Created by serik1987 on 21.09.19.
//

#ifndef MPI2_LOCALMATRIX_H
#define MPI2_LOCALMATRIX_H

#include "Matrix.h"
#include "exceptions.h"

namespace data {

    /**
     * The local matrix. In the local matrix any process has no access to the data calculated by another processes
     *
     * For instance, if you try to call localMatrix[j] the result depends whether j-th item of the mattrix is calculated
     * by the certain process or by another process. If this is calculated by another process an exception will be
     * generated
     *
     * Positives:
     * The matrix requires small amount of memory (N*8/n per process, N*8 per application where N is total number
     * of items in the matrix and n is number of processes).
     *
     * Negatives:
     * Any operations requiring the whole data in the matrix (e.g., transformation, nultiplication etc.) will generate
     * an exception within the local matrix
     */
    class LocalMatrix: public Matrix {
    private:
        void copyData(const LocalMatrix& other);

    public:

        /**
         * Creates the local matrix
         *
         * @param comm communicator responsible for such processes
         * @param width matrix width in pixels
         * @param height matrix height in pixels
         * @param widthUm matrix width in um or any other other native physiological units
         * @param heightUm matrix height in um or any other native physiological units
         * @param filler default value for all matrix items
         */
        LocalMatrix(mpi::Communicator& comm, int width, int height, double widthUm, double heightUm,
                double filler = 0);

        /**
         * Copy constructor
         *
         * @param other the source matrix
         */
        LocalMatrix(const LocalMatrix& other);

        ~LocalMatrix();

        /**
         * Provides an access to the matrix element
         *
         * @param index index of the element
         * @return alias to the element value
         */
        double& operator[](int index) override{
            if (index >= iStart && index < iFinish){
                return data[index - iStart];
            } else if (index >= 0 && index < size){
                throw missed_data_error();
            } else {
                throw out_of_range_error();
            }
        }

        /**
         * Provides an access to the matrix element
         *
         * @param index index of the element
         * @return alias to the element value
         */
        double operator[](int index) const override{
            if (index >= iStart && index < iFinish){
                return data[index - iStart];
            } else if (index >= 0 && index < size){
                throw missed_data_error();
            } else {
                throw out_of_range_error();
            }
        }

        /**
         * Matrix synchronization is not supported for local matrices
         */
        void synchronize(){
            throw synchronization_error();
        }

        /**
         * Matrix synchronization is not supported for for local matrices
         *
         * @param root
         */
        void synchronize(int root){
            throw synchronization_error();
        }

        LocalMatrix& operator=(const LocalMatrix& other);
        LocalMatrix& operator=(LocalMatrix&& other);
    };

}


#endif //MPI2_LOCALMATRIX_H
