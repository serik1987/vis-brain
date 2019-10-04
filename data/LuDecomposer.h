//
// Created by serik1987 on 03.10.19.
//

#ifndef MPI2_LUDECOMPOSER_H
#define MPI2_LUDECOMPOSER_H


#include "../mpi/Communicator.h"
#include "ContiguousMatrix.h"

namespace data {

    /**
     * Solves algebraic equations by means of the LU decomposition technique. The decomposition is fully parallelized
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
         * Performs decomposition of the source matrix on L and U. This method launches automatically when you create
         * an instance of this class. However, if you changed the source matrix and want to get updated values of L
         * and U you have to run this method again.
         */
        void decompose();
    };

}


#endif //MPI2_LUDECOMPOSER_H
