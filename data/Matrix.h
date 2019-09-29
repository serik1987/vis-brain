//
// Created by serik1987 on 21.09.19.
//

#ifndef MPI2_MATRIX_H
#define MPI2_MATRIX_H


#include "../mpi/Communicator.h"
#include "../compile_options.h"
#include "../log/output.h"

namespace data{ class Matrix; }

/**
          * Exchanges values between the matrices
          *
          * @param A the first matrix
          * @param B the second matrix
          */
void swap(data::Matrix& A, data::Matrix& B);

namespace data {

    class ContiguousMatrix;

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

            bool operator<(const AbstractIterator& other){ return index < other.index; }
            bool operator<=(const AbstractIterator& other){ return index <= other.index; }
            bool operator>(const AbstractIterator& other){ return index > other.index; }
            bool operator>=(const AbstractIterator& other){ return index >= other.index; }
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

            Iterator operator+(int n) const{
                Iterator it = *this;
                it.index += n;
                it.pointer += n;
                return it;
            }

            friend Iterator operator+(int n, const Iterator& other){
                return other + n;
            }

            Iterator operator-(int n) const{
                Iterator result = *this;
                result.index -= n;
                result.pointer -= n;
                return result;
            }

            int operator-(const Iterator& other) const{
                return index - other.index;
            }

            Iterator& operator+=(int n){
                index += n;
                pointer += n;
                return *this;
            }

            Iterator& operator-=(int n){
                index -= n;
                pointer -= n;
                return *this;
            }

            double& operator[](int n) const{
                return *(pointer + n);
            }

            /**
             * Access to the element by means of its relative address (no checking) when the iterator is valid
             *
             * @param i relative index of the row (the iterator points to an item with relative index equal to 0)
             * @param j relative index of the column (the iterator points to an item with relative index equal to 0)
             * @return reference to the value
             */
            double& val(int i, int j){
                return *(pointer + parent->width * i + j);
            }

            friend void swap(const Iterator& a, const Iterator& b){
                double x = *a.pointer;
                *a.pointer = *b.pointer;
                *b.pointer = x;
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

             ConstantIterator operator+(int n) const{
                 ConstantIterator it = *this;
                 it.index += n;
                 it.pointer += n;
                 return it;
             }

             friend ConstantIterator operator+(int n, const ConstantIterator& other){
                 return other + n;
             }

             ConstantIterator operator-(int n) const{
                 ConstantIterator x = *this;
                 x.index -= n;
                 x.pointer -= n;
                 return x;
             }

             int operator-(const ConstantIterator& other) const{
                 return index - other.index;
             }

             ConstantIterator& operator+=(int n){
                 index += n;
                 pointer += n;
                 return *this;
             }

             ConstantIterator& operator-=(int n){
                 index -= n;
                 pointer -= n;
                 return *this;
             }

             double operator[](int n) const{
                 return *(pointer + n);
             }

             /**
              * returns value with a certain relative index when the iterator is valid
              *
              * @param i relative index of the row
              * @param j relative index of the column
              * The iterator points to a row and a column which relative indexes are zero
              * @return a copy of the value
              */
             double val(int i, int j) const{
                return *(pointer + parent->width * i + j);
             }
         };

         virtual Iterator begin() = 0;
         virtual Iterator end() = 0;
         virtual ConstantIterator cbegin() const = 0;
         virtual ConstantIterator cend() const = 0;

         /**
          * Exchanges values between the matrices
          *
          * @param A the first matrix
          * @param B the second matrix
          */
         friend void ::swap(Matrix& A, Matrix& B);

         /**
          * Fills all matrix data by a certain value
          *
          * @param x
          */
         void fill(double x);

         /**
          * Fills matrix by means of predefined values
          *
          * @tparam F some functor like double (*F)(data::Matrix::Iterator)
          * @param f the functor instance
          */
         template<typename F> void fill(F f){
             for (auto a = begin(); a != end(); ++a){
                 *a = f(a);
             }
         };

         /**
          * applies some function to the matrix B and puts the results into matrix A
          *
          * @tparam F some functor like double (*F)(const data::Matrix::Iterator&)
          * @param B another matrix
          * @param f instance of the function
          */
         template<typename F> void calculate(const Matrix& B, F f){
            auto a = begin();
            auto b = B.cbegin();
            for (;a != end(); ++a, ++b){
                *a = f(b);
            }
         }

         /**
          * applies some function to matrices B and C and puts the results into current matrix
          *
          * @tparam F some functor like double (*F)(const Iterator&, const Iterator&)
          * @param B the second matrix
          * @param C the third matrix
          * @param f instance of the functor
          */
         template<typename F> void calculate(const Matrix& B, const Matrix& C, F f){
            auto a = begin();
            auto b = B.cbegin();
            auto c = C.cbegin();
            for (; a != end(); ++a, ++b, ++c){
                *a = f(b, c);
            }
         }

         /**
          * reduces all matrix items to a single value using some reduction function.
          * This is a collective routine
          *
          * @tparam F some functor like: void (*F)(double& x, double y)
          * @param x0 initial value
          * @param op MPI_Op instance shall correspond to the functor
          * @param f the reduction function. The reduction will be organized in the following way:
          * s = x0;
          * f(s, A[0, 0])
          * f(s, A[0, 1])
          * f(s, A[0, 2])
          * ...
          */
         template<typename F> double reduce(double x0, MPI_Op op, F f) const{
             double result = 0.0;
             for (auto a = cbegin(); a != cend(); ++a){
                 f(result, *a);
             }
             double global_result = 0.0;
             communicator.allReduce(&result, &global_result, 1, MPI_DOUBLE, op);
             f(global_result, x0);
             return global_result;
         }

         Matrix& operator++();
         Matrix& operator--();
         Matrix& operator+=(const Matrix& other);
         Matrix& operator+=(double x);
         Matrix& operator-=(const Matrix& other);
         Matrix& operator-=(double x);


         /**
          * Performs addition of A and B. The result will be put to the current matrix
          *
          * @param A the first matrix
          * @param B the second matrix
          * @return reference to the current matrix
          */
         Matrix& add(const Matrix& A, const Matrix& B);

         /**
          * Performs A-B. The result will be put to the current matrix
          *
          * @param A the first matrix
          * @param B the second matrix
          * @return reference to the current matrix
          */
         Matrix& sub(const Matrix& A, const Matrix& B);

         /**
          * Performs A+x and puts the result to the current matrix
          *
          * @param A the matrix
          * @param x the number
          * @return reference to this matrix
          */
         Matrix& add(const Matrix& A, double x);

         /**
          * Performs x+A. The result will be put to the current matrix
          *
          * @param x the number
          * @param A the matrix
          * @return reference to the result
          */
         Matrix& add(double x, const Matrix& A) { return add(A, x); }


         /**
          * Provides A-x operation and puts the result to the current matrix
          *
          * @param A the matrix itself
          * @param x substracting number
          * @return reference to the result
          */
         Matrix& sub(const Matrix& A, double x);

         /**
          * Provides x-A operation and puts the result to the current matrix
          *
          * @param x the number
          * @param A the matrix
          * @return reference to the result
          */
         Matrix& sub(double x, const Matrix& A);

         /**
          * Performs -A and puts the result to the current matrix
          *
          * @param A the matrix to negotiate
          * @return reference to the negotiation result
          */
         Matrix& neg(const Matrix& A);

         /**
          * Provides item-by-item multiplication. The results will be returned to the same matrix.
          *
          * @param other the other multiplication term
          * @return reference to the result
          */
         Matrix& operator*=(const Matrix& other);

         /**
          * Multiplies all items in the matrix by the value x
          *
          * @param x the value
          * @return reference to the result
          */
         Matrix& operator*=(double x);

         /**
          * Provides item-by-item division. The results will be returned to the same matrix
          *
          * @param other the other multiplication term
          * @return reference to the result
          */
         Matrix& operator/=(const Matrix& other);

         /**
          * Divides all matrix items by the same value x
          *
          * @param x the value itself
          * @return reference to the result
          */
         Matrix& operator/=(double x);


         /**
          * Provides item-by-item multiplication. Results will be put to the current matrix
          *
          * @param A the first matrix
          * @param B the second matrix
          * @return reference to the result
          */
         Matrix& mul(const Matrix& A, const Matrix& B);


         /**
          * Provides item-by-item division. Rresults will be put to the current matrix
          *
          * @param A The first term
          * @param B The second term
          * @return reference to the result
          */
         Matrix& div(const Matrix& A, const Matrix& B);

         /**
          * Multiplies all items on the matrix A by the same number x and put the results to the current matrix
          *
          * @param A source matrix
          * @param x source number
          * @return reference to the result
          */
         Matrix& mul(const Matrix& A, double x);

         /**
          * Divides all items on the matrix by the same number x and put the results to the current matrix
          *
          * @param A source matrix
          * @param x source number
          * @return reference to the result
          */
         Matrix& div(const Matrix& A, double x);

         /**
          * Multiplies all values of the matrix A by the same value x and puts the results to the current matrix
          *
          * @param x source number
          * @param A  source matrix
          * @return reference to the result
          */
         Matrix& mul(double x, const Matrix& A) { return mul(A, x); }

         /**
          * Divides the value x by all items for the matrix A and puts the results to the current matrix
          *
          * @param x source number
          * @param A source matrix
          * @return reference to the result
          */
         Matrix& div(double x, const Matrix& A);

        /**
         * transposes matrix A and writes transposition result to the current matrix
         * Warning: the function will work correctly only when matrix A is contiguous and synchronized
         * Warning2: if nxm is size of the current matrix, the matrix A shall have size mxn for calculations to be
         * performed correctly
         * This is an unsafe method
         *
         * @param A source matrix
         * @return reference to the result
         */
        virtual Matrix& transpose(const ContiguousMatrix& A);

        /**
         * Products two matrices. In contrast to mul(...) method, it provides true matrix production, onot
         * item-by-item production. Matrix order makes sense.
         * Warning # 1. Both A and B are contiguous synchronized matrices
         * Warning # 2. Real matrix will not be synchronized after accomplishment of this method
         * Warning # 3. Please, be sure that matrix sizes correspond to each other. Otherwise, the results may
         * be incorrect or the program will fail
         *
         * @param A the first matrix
         * @param B the second matrix
         * @return reference to the result
         */
        Matrix& dot(const ContiguousMatrix& A, const ContiguousMatrix& B);
    };

}


#endif //MPI2_MATRIX_H
