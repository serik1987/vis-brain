//
// Created by serik1987 on 21.09.19.
//

#ifndef MPI2_LOCALMATRIX_H
#define MPI2_LOCALMATRIX_H

#include "Matrix.h"
#include "exceptions.h"

namespace data {

    /**
     * Matrix that contains only those data that will be processed by the certain process An instance of this
     * class allocates width*height*8/N bytes of memory for each process where width, heights are matrix dimensions
     * and N is current number of processes within the communicator. The matrix is separated into
     * several parts (so called zones of responsibility. Any process has reading or writing access only to such a
     * piece. Trying to access items without zone responsibily will cause a simulation error
     */
    class LocalMatrix: public Matrix {
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
    };

}


#endif //MPI2_LOCALMATRIX_H
