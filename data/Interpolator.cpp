//
// Created by serik1987 on 15.10.2019.
//

#include "Interpolator.h"
#include "exceptions.h"

#define SQR(x)  x * x
#define CUBE(x) x * x * x

#define F_00    derivatives[0]
#define F_01    derivatives[1]
#define F_10    derivatives[2]
#define F_11    derivatives[3]
#define Fx_00   derivatives[4]
#define Fx_01   derivatives[5]
#define Fx_10   derivatives[6]
#define Fx_11   derivatives[7]
#define Fy_00   derivatives[8]
#define Fy_01   derivatives[9]
#define Fy_10   derivatives[10]
#define Fy_11   derivatives[11]
#define Fxy_00  derivatives[12]
#define Fxy_01  derivatives[13]
#define Fxy_10  derivatives[14]
#define Fxy_11  derivatives[15]


namespace data {

    const int INTERPOLATION_MATRIX_SIZE = 16;
    const double Interpolator::INTERPOLATION_MATRIX[16][16] =
            {{1,    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}, // 0
             {0,    0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}, // 1
             {-3,   3,  0,  0,  -2, -1, 0,  0,  0,  0,  0,  0,  0,  0,  0,  0}, // 2
             {2,    -2, 0,  0,  1,  1,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0}, // 3
             {0,    0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0,  0,  0,  0,  0}, // 4
             {0,    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  0,  0,  0}, // 5
             {0,    0,  0,  0,  0,  0,  0,  0,  -3, 3,  0,  0,  -2, -1, 0,  0}, // 6
             {0,    0,  0,  0,  0,  0,  0,  0,  2,  -2, 0,  0,  1,  1,  0,  0}, // 7
             {-3,   0,  3,  0,  0,  0,  0,  0,  -2, 0,  -1,  0,  0,  0,  0,  0}, // 8
             {0,    0,  0,  0,  -3, 0,  3,  0,  0,  0,  0,  0,  -2, 0,  -1, 0}, // 9
             {9,    -9, -9, 9,  6,  3,  -6, -3, 6,  -6, 3,  -3, 4,  2,  2,  1}, // 10
             {-6,   6,  6,  -6, -3, -3, 3,  3,  -4, 4,  -2, 2,  -2, -2, -1, -1}, // 11
             {2,    0,  -2, 0,  0,  0,  0,  0,  1,  0,  1,  0,  0,  0,  0,  0}, // 12
             {0,    0,  0,  0,  2,  0,  -2, 0,  0,  0,  0,  0,  1,  0,  1,  0}, // 13
             {-6,   6,  6,  -6, -4, -2, 4,  2,  -3, 3,  -3, 3,  -2, -1, -2, -1}, //14
             {4,    -4, -4, 4,  2,  2,  -2, -2, 2,  -2, 2,  -2, 1,  1,  1,  1}}; // 15

    Interpolator::Interpolator(data::Matrix &result, const data::Matrix &source) {
        sourceHeight = source.getHeight();
        sourceWidth = source.getWidth();
        resultHeight = result.getHeight();
        resultWidth = result.getWidth();
        double Nx_float = resultWidth / sourceWidth;    Nx = (int)Nx_float;
        double Ny_float = resultHeight / sourceHeight;  Ny = (int)Ny_float;
        if (Nx != Nx_float || Ny != Ny_float || sourceWidth == 1 || sourceHeight == 1){
            throw matrix_dimensions_mismatch();
        }
        nprocs = result.getCommunicator().getProcessorNumber();
        rank = result.getCommunicator().getRank();
        localRows = sourceHeight / nprocs / (sourceHeight % nprocs != 0);
        rowsToGather = localRows * nprocs;
        recvbufSize = rowsToGather * Ny * sourceWidth * Nx;
        sendCount = localRows * Ny * sourceWidth * Nx;
        startRow = localRows * rank;
        finishRow = startRow + localRows;
        if (finishRow > sourceHeight){
            finishRow = sourceHeight;
            localRows = finishRow - startRow;
        }
        recvbuf = new double[recvbufSize];
    }

    Interpolator::~Interpolator(){
        delete [] recvbuf;
    }

    void Interpolator::interpolate(data::ContiguousMatrix &result, const data::ContiguousMatrix &source) {
        /* The interpolation algorithm is described here: https://en.wikipedia.org/wiki/Bicubic_interpolation */
        checkInterpolationMatrices(result, source);
        for (int i0 = startRow; i0 < finishRow; ++i0){
            for (int j0 = 0; j0 < sourceWidth; ++j0){
                ContiguousMatrix::ConstantIterator F(source, i0, j0);
                ContiguousMatrix::Iterator G(result, i0 * Ny, j0 * Nx);
                calculateFunctionValues(i0, j0, F);
                calculateDerivatives(F);
                calculateCoefficients();
                fillInterpolationValues(G);
            }
        }
        synchronizeResultMatrix(result);
    }

    inline void Interpolator::checkInterpolationMatrices(const data::ContiguousMatrix &result,
                                                  const data::ContiguousMatrix &source) {
        if (result.getHeight() != resultHeight || result.getWidth() != resultWidth ||
            source.getHeight() != sourceHeight || source.getWidth() != sourceWidth){
            throw matrix_dimensions_mismatch();
        }
    }

    inline void Interpolator::calculateFunctionValues(int i0, int j0, const data::ContiguousMatrix::ConstantIterator &F) {
        /* If some function values are present the will be simply copied into object private variables.
         * If they are missing, they will be calculated based on the function slope
         */
        F_00 = F.val(0, 0);
        if (i0 == sourceHeight - 1){ // # rows > 1 guarantees that i0 >= 1 => valid row and i0-1 >= 0 => valid row
            F_10 = 2 * F.val(0, 0) - F.val(-1, 0);
        } else {
            F_10 = F.val(1, 0);
        }
        if (i0 >= sourceHeight - 2){
            F_20 = 2 * F_10 - F_00;
        } else {
            F_20 =F.val(2, 0);
        }
        if (i0 == 0){
            F_m10 = 2 * F_00 - F_10;
        } else {
            F_m10 = F.val(-1, 0);
        }
        if (j0 == sourceWidth -1){ // # cols > 1 guarantees that j0 >= 1 => valid col, j0-1 >= 0 => valid col
            F_01 = 2 * F.val(0, 0) - F.val(0, -1);
        } else {
            F_01 = F.val(0, 1);
        }
        if (i0 == sourceHeight - 1 && j0 == sourceWidth - 1){ // the last item
            F_11 = 2 * F.val(0, 0) - F.val(-1, -1);
        } else if (j0 == sourceWidth - 1){ // the last col but not row
            F_11 = 2 * F.val(1, 0) - F.val(1, -1);
        } else if (i0 == sourceHeight - 1){ // the last row but not col
            F_11 = 2 * F.val(0, 1) - F.val(-1, 1);
        } else {
            F_11 = F.val(1, 1);
        }
        if (i0 >= sourceHeight - 2){
            F_21 = 2 * F_11 - F_10;
        } else {
            F_21 = F.val(2, 1);
        }
        if (i0 == 0){
            F_m11 = 2 * F_01 - F_11;
        } else {
            F_m11 = F.val(-1, 1);
        }
        if (j0 >= sourceWidth - 2){
            F_m12 = 2 * F_m11 - F_m10;
            F_02 = 2 * F_01 - F_00;
            F_12 = 2 * F_11 - F_10;
            F_22 = 2 * F_21 - F_20;
        } else {
            F_m12 = F.val(-1, 2);
            F_02 = F.val(0, 2);
            F_12 = F.val(1, 2);
            F_22 = F.val(2, 2);
        }
        if (j0 == 0){
            F_m1m1 = 2 * F_m10 - F_m11;
            F_0m1 = 2 * F_00 - F_01;
            F_1m1 = 2 * F_10 - F_11;
            F_2m1 = 2 * F_20 - F_21;
        } else {
            F_m1m1 = F.val(-1, -1);
            F_0m1 = F.val(0, -1);
            F_1m1 = F.val(1, -1);
            F_2m1 = F.val(2, -1);
        }
    }

    inline void Interpolator::calculateDerivatives(const data::ContiguousMatrix::ConstantIterator &F) {
        /* We represent the 1st derivative in similar manner */
        Fy_00 = 0.5 * (F_10 - F_m10);   Fy_01 = 0.5 * (F_11 - F_m11);
        Fy_10 = 0.5 * (F_20 - F_00);    Fy_11 = 0.5 * (F_21 - F_01);

        /* Matrix for the 'x derivative is a bit larger */
        double Fx_m10, Fx_20, Fx_m11, Fx_21;
        Fx_m10 = 0.5 * (F_m11 - F_m1m1);    Fx_m11 = 0.5 * (F_m12 - F_m10);
        Fx_00 = 0.5 * (F_01 - F_0m1);       Fx_01 = 0.5 * (F_02 - F_00);
        Fx_10 = 0.5 * (F_11 - F_1m1);       Fx_11 = 0.5 * (F_12 - F_10);
        Fx_20 = 0.5 * (F_21 - F_2m1);       Fx_21 = 0.5 * (F_22 - F_20);

        Fxy_00 = 0.5 * (Fx_10 - Fx_m10);    Fxy_01 = 0.5 * (Fx_11 - Fx_m11);
        Fxy_10 = 0.5 * (Fx_20 - Fx_00);     Fxy_11 = 0.5 * (Fx_21 - Fx_01);
    }

    inline void Interpolator::calculateCoefficients() {
        int local_row = 0, local_col = 0;
        for (int i=0; i < 16; ++i){
            double& coefficients = a[local_row][local_col] = 0.0;
            for (int j=0; j < 16; ++j){
                coefficients += INTERPOLATION_MATRIX[i][j] * derivatives[j];
            }
            ++local_col;
            if (local_col == 4){
                local_col = 0;
                ++local_row;
            }
        }
    }

    inline void Interpolator::fillInterpolationValues(data::ContiguousMatrix::Iterator &G) {
        for (int i = 0; i < Ny; ++i){
            for (int j = 0; j < Nx; ++j){
                double row = (double)i/Ny;
                double col = (double)j/Nx;
                G.val(i, j) =   a[0][0]             +   a[0][1] * col               +   a[0][2] * SQR(col)              +   a[0][3] * CUBE(col) +
                                a[1][0] * row       +   a[1][1] * row * col         +   a[1][2] * row * SQR(col)        +   a[1][3] * row * CUBE(col) +
                                a[2][0] * SQR(row)  +   a[2][1] * SQR(row) * col    +   a[2][2] * SQR(row) * SQR(col)   +   a[2][3] * SQR(row) * CUBE(col) +
                                a[3][0] * CUBE(row) +   a[3][1] * CUBE(row) * col   +   a[3][2] * CUBE(row) * SQR(col)  +   a[3][3] * CUBE(row) * CUBE(col);
            }
        }
    }

    inline void Interpolator::synchronizeResultMatrix(data::ContiguousMatrix &result) {
        ContiguousMatrix::Iterator G(result, startRow * Ny, 0);
        ContiguousMatrix::Iterator G0(result, 0, 0);
        double* sendbuf = &(*G);
        result.getCommunicator().allGather(sendbuf, sendCount, MPI_DOUBLE, recvbuf, sendCount, MPI_DOUBLE);
        for (int i=0; i < result.getSize(); ++i, ++G0){
            *G0 = recvbuf[i];
        }
    }


}