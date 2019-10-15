//
// Created by serik1987 on 15.10.2019.
//

#ifndef MPI2_INTERPOLATOR_H
#define MPI2_INTERPOLATOR_H

#include "ContiguousMatrix.h"

namespace data {

    /**
     * An auxiliary class that provides matrix interpolation
     */
    class Interpolator {
    private:
        int sourceHeight, sourceWidth, resultHeight, resultWidth;
        int Nx, Ny;

        int nprocs, rank, startRow, finishRow;
        int localRows, rowsToGather, recvbufSize, sendCount;
        double* recvbuf;

        double          F_20, F_m10;
        double          F_21, F_m11;
        double F_02,    F_12, F_m12;
        double F_22, F_m1m1, F_0m1, F_1m1, F_2m1;

        static const int INTERPOLATION_MATRIX_SIZE;
        static const double INTERPOLATION_MATRIX[16][16];
        double derivatives[16];
        double a[4][4];

        /* Some primitive routines */
        inline void checkInterpolationMatrices(const ContiguousMatrix& result, const ContiguousMatrix& source);
        inline void calculateFunctionValues(int i0, int j0, const data::ContiguousMatrix::ConstantIterator& F);
        inline void calculateDerivatives(const data::ContiguousMatrix::ConstantIterator& F);
        inline void calculateCoefficients();
        inline void fillInterpolationValues(data::ContiguousMatrix::Iterator& G);
        inline void synchronizeResultMatrix(ContiguousMatrix& result);
    public:
        /**
         * Constructs the method, but doesn't provide the interpolation itself. Since the resultant and the source
         * matrix is given, the interpolator instance will interpolate all matrices with the same size as the source
         * matrix. The resultant matrix will have the same size as the result.
         *
         * @param result shall be given to define the size of all result matrices
         * @param source shall be given to define the size of all source matrices
         */
        Interpolator(Matrix& result, const Matrix& source);

        Interpolator(const Interpolator& other) = delete;

        ~Interpolator();

        /**
         * Provides an interpolation process.
         * This is a collective routine. It shall be called by all processes simultaneously
         * The source matrix is assumed to be synchronized. See data::ContiguousMatrix::synchronize for details.
         * The result matrix will be synchronized during the interpolation process
         *
         * @param result the result matrix
         * @param source the source matrix
         */
        void interpolate(ContiguousMatrix& result, const ContiguousMatrix& source);
    };

}


#endif //MPI2_INTERPOLATOR_H
