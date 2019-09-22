//
// Created by serik1987 on 21.09.19.
//

#ifndef MPI2_MATRIX_H
#define MPI2_MATRIX_H


#include "../mpi/Communicator.h"
#include "../compile_options.h"

namespace data {

/**
 * A base class for all matrices shared among all processes
 * Minimum total size of each matrix is 8*width*height bytes
 *
 * These matrix are shared. This means that matrix operations were made by certain set of processes
 * in parallel
 */
    class Matrix {
    protected:
        int size, localSize, iStart, iFinish, rank, width, height;
        mpi::Communicator &communicator;
        double widthUm, heightUm;
        double *data;
    public:
        /**
         * Initializes the matrix
         *
         * @param comm communicator responsible for the matrix operations
         * @param w number of samples per width
         * @param h number of samples per height
         * @param w_um width of the matrix in um
         * @param h_um height of the matrix per um
         * @param fillter initial values for the matrix
         */
        Matrix(mpi::Communicator &comm, int w, int h, double w_um, double h_um, double filler = 0.0);

        /**
         *
         * @return total number of samples in the matrix
         */
        int getSize() const { return size;}

        /**
         *
         * @return total number of samples which analyzed by the current process
         */
        int getLocalSize() const { return localSize; }

        /**
         *
         * @return the minimum index of the element processes by the matrix
         */
        int getIstart() const { return iStart; }

        /**
         *
         * @return the maximum index of the element processed by the matrix
         */
        int getIfinish() const { return iFinish; }

        /**
         *
         * @return rank of the process
         */
        int getRank() const { return rank;}

        mpi::Communicator& getCommunicator(){
            return communicator;
        }


        /**
         *
         * @return the matrix width in pixels
         */
        int getWidth() const { return width;}


        /**
         *
         * @return the matrix height in pixels
         */
        int getHeight() const { return height; }


        /**
         *
         * @return the matrix width in um
         */
        double getWidthUm() const {return widthUm; }


        /**
         *
         * @return the matrix height in um
         */
        double getHeightUm() const { return heightUm; }


        /**
         *
         * @return resolution on X (um/pix)
         */
        double getResolutionX() const { return widthUm / width; }

        /**
         *
         * @return resolution on Y (um/pix)
         */
        double getResolutionY() const { return heightUm / height; }

        /**
         * Provides an access to the certain element of the matrix
         *
         * @param index index of such an element
         * @return alias for the element value
         */
        virtual double& operator[](int index) = 0;
        virtual double operator[](int index) const = 0;

        /**
         * An alias for operator[]
         *
         * @param index
         * @return
         */
        double& getValue(int index) { return operator[](index); }

        double getValue(int index) const{ return operator[](index); }

        /**
         * Returns an item with a certain 2D index
         *
         * @param i index of the row
         * @param j index of the column
         * @return
         */
        double& getValue(int i, int j){
            return getValue(i * width + j);
        }

        double getValue(int i, int j) const{
            return getValue(i * width + j);
        }

        /**
         * Updates the data: copies the data set by all processes to all processes. After the synchronization
         * routine any process may access the newest version of any element created by any process.
         *
         * Not available for local matrices
         */
        virtual void synchronize() = 0;

        /**
         * Updates the data: copies the data set by all processes to so called "root process". After the synchronization
         * routine the "root process" may access the newest version of any elements created by any process
         *
         * @param root rank of the "root" process
         */
        virtual void synchronize(int root) = 0;


#if DEBUG==1
        /**
         * Prints the matrix locally. Please, be sure that call of the function is enveloped by logging::enter()
         * and logging::exit()
         */
        void printLocal() const;
#endif
    };

}


#endif //MPI2_MATRIX_H
