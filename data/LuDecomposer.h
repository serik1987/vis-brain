//
// (C) Sergei Kozhukhov, 2019
// (C) the Institute of Higher Nervous Activity and Neurophysiology, Russian Academy of Sciences, 2019
//

#ifndef MPI2_LUDECOMPOSER_H
#define MPI2_LUDECOMPOSER_H


#include "../mpi/Communicator.h"
#include "ContiguousMatrix.h"

namespace data {

    /**
     * Solves algebraic equations by means of the LU decomposition technique. The decomposition is fully parallelized
     *
     * The LU decomposition represents source matrix A as the following matrix production:
     * L*U = P*A
     */
    class LuDecomposer {
    private:
        mpi::Communicator& comm;
        const ContiguousMatrix& A;
        ContiguousMatrix::Iterator _a;
        std::string prompt;
        int interval;
        double* decomposingData;
        int* P;
        double computationTime, transmissionTime;
        double* lBest;
        double* uBest;
        double* uLocal;
        double* uTotal;
        int* pBest;

        double* lBestLocal;
        double uBestLocal;
        int pBestLocal;

        int pStart;
        int pFinish;
        int pConsequtive;

        int jStart, jFinish;
        int localJob, totalJob;
        int M;
        bool bufferLocalInitialized, resultLocalInitialized, resultInitialized;
        double *bufferLocal, *resultLocal, *result;
        mpi::VectorDatatype* dtype;

        void initializeP();

        int N, n, r, i, j, p;

        double& l(int row, int col) const{
            return decomposingData[row * N + col];
        }

        double& u(int row, int col) const{
            return decomposingData[row * N + col];
        }

        double a(int row, int col){
            return _a.val(row, col);
        }

        double pa(int row, int col){
            return _a.val(P[row], col);
        }


#if DEBUG==1
    void getUmax(double& value, int& row, int & col);
    void getUmin(double& value, int& row, int & col);
    void getLmax(double& value, int& row, int & col);
    void getLmin(double& value, int& row, int & col);

    int getLfilled();
    int getUfilled();
#endif

    /**
     * Routines that perform certain stages of the matrix decomposition
     */
    void decomposeFirstRow();
    void searchMaxDiagonal(double& value, int& row);
    void chooseBest(double uBest_local, int bBest_local);
    void initU();

    void checkComputationTime();
    void checkTransmissionTime();

    void computeLBestParallel();
    void computeLConsequtive(int p0);
    void computeUConsequtive();
    void computeUDiagonal(int p0);
    void checkUBestLocal(int p0);
    void chooseLBest();
    void transmitU(int load);

    /**
     * Routines that provide certain steps of the solve(...) method
     */
    void solve_lowerTriangle(double* buf, const ContiguousMatrix& b) const;
    void solve_upperTriangle(ContiguousMatrix& x, const double* buf) const;

    /**
     * Routines for matrix division
     */
     inline void divide_checkMatrices(const ContiguousMatrix& X, const ContiguousMatrix& B) const;
     inline void divide_init(const ContiguousMatrix& B);
     inline void divide_initBuffers(const ContiguousMatrix& B, double* buffer_local,
             double* result_local, double* result);
     inline void divide_clearBuffers();
     inline void divide_lowerTriangle(const ContiguousMatrix& B);
     inline void divide_upperTriangle();
     inline void divide_gather(const double* localBuffer, double* buffer);
     inline void divide_setResult(ContiguousMatrix& X, double* result);

     /**
      * Additional routines for matrix inversion
      */
      inline void inverse_checkMatrix(const ContiguousMatrix& X) const;
      inline void inverse_lowerTriangle();

    public:
        /**
         * Initializes the LU decomposer and provides the LU decomposition process using MPI routines.
         *
         * @param source source matrix (an instance of data::ContiguousMatrix). The matrix is implied to be synchronized
         * so this is your responsibility to achieve this. See data::ContiguousMatrix::synchronize for more details
         * @param userPrompt Give the prompt to the decomposition process each
         * @param userInterval Number of rows after which the progress bar shall be updated
         */
        LuDecomposer(ContiguousMatrix& source, const std::string& userPrompt = "", int userInterval = 10);

        LuDecomposer(const LuDecomposer& other) = delete;

        ~LuDecomposer();

#if DEBUG==1
        void printDebugInformation();
#endif

        /**
         * Returns the value of the lower triangular matrix. The value is shared among all processes.
         *
         * @param row the vertical index of the matrix
         * @param col the horizontal index of the matrix
         * @return copy of the item value
         */
        double L(int row, int col) const{
            if (row > col){
                return l(row, col);
            } else if (row == col){
                return 1.0;
            } else {
                return 0.0;
            }
        }

        /**
         * Returns the value of an item of upper triangular matrix. The value is shared among all processes
         *
         * @param row the vertical index of the item
         * @param col the horizontal index of the item
         * @return copy of the item value
         */
        double U(int row, int col) const{
            if (row > col){
                return 0.0;
            } else {
                return u(row, col);
            }
        }

        /**
         * Returns the value of the matrix PA
         *
         * @param row number of row
         * @param col number of column
         * @return the value itself
         */
        double PA(int row, int col){
            return pa(row, col);
        }

        /**
         *
         * @return L matrix from the LU decomposition
         * New memory is allocated during execution of this routine
         * This is a collective routine
         */
        ContiguousMatrix getLowerTriangle() const;

        /**
         *
         * @return U matrix from the LU decomposition
         * New memory is allocated during execution of this routine
         * This is a collective routine
         */
        ContiguousMatrix getUpperTriangle() const;


        /**
         * Fills the P matrix from the LU decomposition
         * The memory is allocated during execution of this routine
         * This is a collective routine
         *
         * @return the P matrix
         */
        ContiguousMatrix getPermutationMatrix() const;



        /**
         * Performs decomposition of the source matrix on L and U. This method launches automatically when you create
         * an instance of this class. However, if you changed the source matrix and want to get updated values of L
         * and U you have to run this method again.
         */
        void decompose();


        /**
         * Provides solution of A*x = b equation based on the LU decomposition
         * The following arguments were given based on consideration that the decomposer was created for NxN matrix
         *
         * @param x the 1xN matrix that will be filled by the solution values due to the method execution
         * @param b the right-hand side (Nx1 matrix)
         * @param root rank of the root process. Because the job can't be parallelized, it will be run by a single process
         * only. All other processes within the communicator will do nothing and leave vector x unchanged
         * @param buf N-sized C array. The array will be used for storage of temporary values. These values will be
         * non-relevant after the method execution will be finished. If you pass nullptr, the buffer will be created
         * at the beginning of the method execution and will be destroyed at finish of it
         */
        void solve(ContiguousMatrix& x, const ContiguousMatrix& b, int root = 0, double* buf = nullptr) const;

        /**
         * Searches for such matrix X as A*X = B where A is the matrix that has already been decomposed,
         * B is a matrix passed through an argument and * is matrix production.
         * This is a collective routine.
         * Valid results are guaranteed only when the matrix B is synchronized. See ContiguousMatrix::synchronize
         * for details.
         * The result matrix X will be synchronized immediately after this method completes its execution
         *
         * @param X see above
         * @param B see above
         * @param buffer_local 1) if nullptr will be given, the program will allocate the necessary buffer at the
         * beginning of the program execution and will free this at the end of it. This option gives the best
         * performance when you apply this method once. 2) you allocate the local buffer separately by calling
         * divide_createBufferLocal() routine. In this case the program will use the buffer provided by this routine
         * and the buffer will not be destroyed automatically. This option gives the best performance for multiple
         * calls of the divide(...) method
         * @param result_local 1) if nullptr will be given, the program will allocate the necessary buffer at the
         * beginning of the program execution and will free this at the end of it. This option gives the best
         * performance when you apply this method once. 2) you allocate the local result buffer separately by calling
         * divide_createResultLocal() routine. In this case the program will use the buffer provided by this routine
         * and the buffer will not be destroyed automatically. This option gives the best performance for multiple
         * calls of the divide(...) method
         * @param result 1) if nullptr will be given, the program will allocate the necessary buffer at the
         * beginning of the program execution and will free this at the end of it. This option gives the best
         * performance when you apply this method once. 2) you allocate the result buffer separately by calling
         * divide_createResult() routine. In this case the program will use the buffer provided by this routine
         * and the buffer will not be destroyed automatically. This option gives the best performance for multiple
         * calls of the divide(...) method
         *
         * So, there are two ways to use this method.
         * Single-call way.
         * data::LuDedcomposer decomposer(A)
         * decomposer.divide(X, B)
         *
         * Multiple-call way.
         * data::LuDecomposer decomposer(A)
         * auto buffer_local = decomposer.divide_createBufferLocal();
         * auto result_local = decomposer.divide_createResultLocal();
         * auto result = decomposer.divide_createResult();
         * decomposer.divide(X1, B1, buffer_local, buffer, result_local, result, dtype);
         * decomposer.divide(X2, B2, buffer_local, buffer, result_local, result, dtype);
         * decomposer.divide(X3, B3, buffer_local, buffer, result_local, result, dtype);
         * ......................................
         * decomposer.divide(Xn, Bn, buffer_local, buffer, result_local, result, dtype);
         * delete [] buffer_local;
         * delete [] result_local;
         * delete [] result;
         */
        void divide(ContiguousMatrix& X, const ContiguousMatrix& B, double* buffer_local = nullptr,
                double* result_local = nullptr, double* result = nullptr);

        /**
         * Searches for the inverse matrix X = A^-1
         *
         * @param X the inverse results
         * @param buffer_local 1) if nullptr will be given, the program will allocate the necessary buffer at the
         * beginning of the program execution and will free this at the end of it. This option gives the best
         * performance when you apply this method once. 2) you allocate the local buffer separately by calling
         * divide_createBufferLocal() routine. In this case the program will use the buffer provided by this routine
         * and the buffer will not be destroyed automatically. This option gives the best performance for multiple
         * calls of the divide(...) method
         * @param result_local 1) if nullptr will be given, the program will allocate the necessary buffer at the
         * beginning of the program execution and will free this at the end of it. This option gives the best
         * performance when you apply this method once. 2) you allocate the local result buffer separately by calling
         * divide_createResultLocal() routine. In this case the program will use the buffer provided by this routine
         * and the buffer will not be destroyed automatically. This option gives the best performance for multiple
         * calls of the divide(...) method
         * @param result 1) if nullptr will be given, the program will allocate the necessary buffer at the
         * beginning of the program execution and will free this at the end of it. This option gives the best
         * performance when you apply this method once. 2) you allocate the result buffer separately by calling
         * divide_createResult() routine. In this case the program will use the buffer provided by this routine
         * and the buffer will not be destroyed automatically. This option gives the best performance for multiple
         * calls of the divide(...) method
         */
        void inverse(ContiguousMatrix& X, double* buffer_local = nullptr, double* result_local = nullptr,
                double* result = nullptr);

        /**
         * Creates the local buffer for the division process
         *
         * @param B matrix that has the same dimensions as the matrix B in equation A*X = B
         * @param initialized for service use only. Please, use the default value
         * @return pointer to the buffer. Please, don't forget to clean the buffer manually after the program execution
         * will be completed
         */
        inline double* divide_createBufferLocal(const ContiguousMatrix& B, bool initialized = false);

        /**
         * Creates the local result buffer for the division process
         *
         * @param B matrix that has the same dimensions as the matrix B in equation A*X = B
         * @param initialized for service use only. Please, use the default value
         * @return pointer to the buffer. Please, don't forget to clean the buffer manually after the program execution
         * will be completed
         */
        inline double* divide_createResultLocal(const ContiguousMatrix& B, bool initialized = false);

        /**
         * Creates the result buffer for the division process
         *
         * @param B matrix that has the same dimensions as the matrix B in equation A*X = B
         * @param initialized for service use only. Please, use the default value
         * @return pointer to the buffer. Please, don't forget to clean the buffer manually after the program execution
         * will be completed
         */
        inline double* divide_createResult(const ContiguousMatrix& B, bool initialized = false);
    };

}


#endif //MPI2_LUDECOMPOSER_H
