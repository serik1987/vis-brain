//
// Created by serik1987 on 21.09.19.
//

#ifndef MPI2_MATRIX_H
#define MPI2_MATRIX_H


#include "../mpi/Communicator.h"
#include "../compile_options.h"
#include "../log/output.h"

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









        /**
         * Matrix iterators
         */

        /**
         * Parent for all iterators in the matrix
         */
        class AbstractIterator{
        protected:
            int index = -1;
        public:

            /**
             * Single-value constructor
             *
             * @param idx index for the matrix
             */
            AbstractIterator(int idx): index(idx) {};

            /**
             *
             * @return current index of the iterator
             */
            int getIndex() const { return index;}

            /**
             *
             * @return current row for the matrix
             */
            virtual int getRow() const = 0;

            /**
             *
             * @return current column for the matrix
             */
            virtual int getColumn() const = 0;

            /**
             *
             * @return current row for the matrix in um or any other suitable units
             */
            virtual double getRowUm() const = 0;

            /**
             *
             * @return current column for the matrix in um or any other suitable units
             */
            virtual double getColumnUm() const = 0;
        };

        /**
         * Common parent for all read-and-write iterators
         */
        class Iterator: public virtual AbstractIterator{
        protected:
            Matrix* parent;
            double* pointer;
        public:
            /**
             * Single-value constructor
             *
             * @param matrix alias to the matrix which iterator shall be taken into consideration
             * @param index index for the item
             */
            Iterator(Matrix& matrix, int index): AbstractIterator(index){
                parent = &matrix;
                pointer = &parent->data[index];
            }

            /**
             *
             * @return current row
             */
            virtual int getRow() const override{
                return index / parent->width;
            }

            /**
             *
             * @return current column for the matrix
             */
            virtual int getColumn() const override{
                return index % parent->width;
            }

            /**
             *
             * @return current row for the matrix in um or any other suitable units
             */
            virtual double getRowUm() const override{
                return (parent->height/2 - getRow()) * parent->heightUm / (parent->height - 1);
            }

            /**
             *
             * @return current column for the matrix in um or any other suitable units
             */
            virtual double getColumnUm() const override{
                return (getColumn() - parent->width/2) * parent->widthUm / (parent->width - 1);
            };

            Iterator& operator=(const Iterator& other){
                index = other.index;
                parent = other.parent;
                pointer = other.pointer;
                return *this;
            }

            bool operator==(const Iterator& other) const{
                return pointer == other.pointer;
            }

            bool operator!=(const Iterator& other) const{
                return pointer != other.pointer;
            }

            double& operator*() const{ return *pointer; }

            Iterator& operator++() {
                ++index;
                ++pointer;
                return *this;
            }

            Iterator operator++(int){
                Iterator it = *this;
                ++index;
                ++pointer;
                return it;
            }

            Iterator& operator--() {
                --index;
                --pointer;
                return *this;
            }

            Iterator operator--(int){
                Iterator it = *this;
                --index;
                --pointer;
                return it;
            }
        };

        /**
         * Common parent for all read-only iterators
         */
         class ConstantIterator: public virtual AbstractIterator{
         protected:
             const Matrix* parent;
             const double* pointer;
         public:

             /**
              * Single-value constructor
              *
              * @param matrix matrix alias to the matrix which iterator shall be taken into consideration
              * @param index index for the item
              */
             ConstantIterator(const Matrix& matrix, int index): AbstractIterator(index){
                parent = &matrix;
                pointer = &parent->data[index];
             }

             /**
              *
              * @return current row
              */
             virtual int getRow() const override {
                 return index / parent->width;
             }

             /**
              *
              * @return
              */
             virtual int getColumn() const override {
                 return index % parent->width;
             }

             /**
             *
             * @return current row for the matrix in um or any other suitable units
             */
             virtual double getRowUm() const override{
                 return (parent->height/2 - getRow()) * parent->heightUm / (parent->height - 1);
             }

             /**
             *
             * @return current column for the matrix in um or any other suitable units
             */
             virtual double getColumnUm() const override{
                 return (getColumn() - parent->width/2) * parent->widthUm / (parent->width - 1);
             };

             ConstantIterator& operator=(const ConstantIterator& other){
                 index = other.index;
                 parent = other.parent;
                 pointer = other.pointer;
                 return *this;
             }

             bool operator==(const ConstantIterator& other) const{
                 return pointer == other.pointer;
             }

             bool operator!=(const ConstantIterator& other) const{
                 return pointer != other.pointer;
             }

             double operator*() const{ return *pointer; }

             ConstantIterator& operator++(){
                 ++index;
                 ++pointer;
                 return *this;
             }

             ConstantIterator operator++(int){
                 ConstantIterator it = *this;
                 ++index;
                 ++pointer;
                 return it;
             }

             ConstantIterator& operator--(){
                 --index;
                 --pointer;
                 return *this;
             }

             ConstantIterator operator--(int){
                 ConstantIterator it = *this;
                 --index;
                 --pointer;
                 return it;
             }
         };
    };

}


#endif //MPI2_MATRIX_H
