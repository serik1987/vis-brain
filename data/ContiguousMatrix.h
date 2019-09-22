//
// Created by serik1987 on 21.09.19.
//

#ifndef MPI2_CONTIGUOUSMATRIX_H
#define MPI2_CONTIGUOUSMATRIX_H


#include "../mpi/Communicator.h"
#include "Matrix.h"
#include "exceptions.h"

namespace data {

    /**
     * Represents so called contiguous matrix. A contiguous matrix allocates 8*w*h bytes of memory for each process
     * where w is matrix width and h is matrix height. The whole matrix is separated into N pieces where N is
     * number of processes in the communicator. Each piece is so called responsibility area for a certain process.
     * Any matrix operations write the data only to the responsibility area. Each process may access the whole matrix
     * but the data outside the responsibility area may be old. Special synchronization routines were applied in order
     * to exchange the data between the processes. After synchronization routines all data within the responsibility
     * area and outside are up to date.
     *
     * Example 1.
     * matrix.getValue(i, j); // Returns the last version of the data only when you access the responsibility area
     *
     * Example 2.
     * matrix.synchronize();
     * matrix.getValue(i, j); // Returns the last version of the data irrespectively of their location within the matrix
     *
     * Example 3.
     * matrix.synchronize();
     * matrix = B*C;
     * matrix.getValue(i, j); // The data outside the responsibility area are deprecated because the data were changed
     * after last synchronization
     *
     * Use matrix iterator to ensure that you always access the responsibility area
     *
     * The object requires synchronization buffer. Its size is as many as the size of the matrix
     */
    class ContiguousMatrix: public Matrix {
    private:
        double* bigData;
        double* synchronizationBuffer;

        int allocatorSize;

        void createBuffers();
        void deleteBuffers();
    public:

        /**
         * Initializing the matrix
         *
         * @param comm communicator responsible for the matrix operations
         * @param width matrix width in pixels
         * @param height matrix height in pixels
         * @param widthUm matrix width in Um or any other units
         * @param heightUm matrix height in um or any other units
         * @param filler default value for all data within the matrix
         */
        ContiguousMatrix(mpi::Communicator& comm, int width, int height, double widthUm, double heightUm,
                double filler = 0.0);
        ~ContiguousMatrix();

        ContiguousMatrix(const ContiguousMatrix& other);

        /**
         * Provides an access to the matrix element with a specified index
         *
         * @param index index of an item to access
         * @return the item alias
         */
        double& operator[](int index){
            if (index >= 0 && index < size) {
                return bigData[index];
            } else {
                throw out_of_range_error();
            }
        }

        double operator[](int index) const{
            if (index >= 0 && index < size) {
                return bigData[index];
            } else {
                throw out_of_range_error();
            }
        }

        ContiguousMatrix& operator=(const ContiguousMatrix& other);
        ContiguousMatrix& operator=(ContiguousMatrix&& other);



        /**
         * Synchronization. Synchronization sends the last version of the data from all processes to the root process.
         * After application of this function the root processes has the last version of the data everywhere within
         * the matrix while all other processes have the last version of the data only within their responsibility area
         *
         * WARNING. Synchronization make all iterators incorrect
         * Collective routine
         *
         * @param root rank of the root process in the communicator used for the matrix creation
         */
        void synchronize(int root);


        /**
         * Synchronization. Synchronization sends the recent version of all matrix data to all processes responsible
         * for the matrix
         *
         * WARNING. Synchronization makes all iterators incorrect
         * Collective routine
         */
        void synchronize();
    };

}


#endif //MPI2_CONTIGUOUSMATRIX_H
