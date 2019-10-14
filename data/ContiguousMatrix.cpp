//
// (C) Sergei Kozhukhov, 2019
// (C) the Institute of Higher Nervous Activity and Neurophysiology, Russian Academy of Sciences, 2019
//

#include "ContiguousMatrix.h"
#include "../Application.h"

namespace data{

    ContiguousMatrix::ContiguousMatrix(mpi::Communicator &comm, int width, int height, double widthUm, double heightUm,
                                       double filler):
                                       Matrix(comm, width, height, widthUm, heightUm, filler){
        createBuffers();
        for (int i=0; i < size; i++){
            bigData[i] = filler;
        }
    }

    void ContiguousMatrix::createBuffers(){
        allocatorSize = ceil((double)size/communicator.getProcessorNumber());
        bigData = new double[allocatorSize * communicator.getProcessorNumber()];
        synchronizationBuffer = new double[allocatorSize * communicator.getProcessorNumber()];
        data = bigData + iStart;
    }

    void ContiguousMatrix::deleteBuffers(){
        if (bigData != nullptr) {
            delete[] bigData;
            delete[] synchronizationBuffer;
            bigData = nullptr;
            synchronizationBuffer = nullptr;
        }
    }

    ContiguousMatrix::ContiguousMatrix(const ContiguousMatrix &other):
        Matrix(other.communicator, other.width, other.height, other.widthUm, other.heightUm){
        createBuffers();
        memcpy(data, other.data, localSize * sizeof(double));
    }

    ContiguousMatrix::~ContiguousMatrix() {
        deleteBuffers();
    }

    void ContiguousMatrix::synchronize(int root) {
        double* temporaryBuffer;
        communicator.gather(data, allocatorSize, MPI_DOUBLE, synchronizationBuffer, allocatorSize, MPI_DOUBLE, root);
        if (rank == root) {
            temporaryBuffer = bigData;
            bigData = synchronizationBuffer;
            synchronizationBuffer = temporaryBuffer;
            data = bigData + iStart;
        }
    }

    void ContiguousMatrix::synchronize() {
        double* temporaryBuffer;
        communicator.allGather(data, allocatorSize, MPI_DOUBLE, synchronizationBuffer, allocatorSize, MPI_DOUBLE);
        temporaryBuffer = bigData;
        bigData = synchronizationBuffer;
        synchronizationBuffer = temporaryBuffer;
        data = bigData + iStart;
    }

    ContiguousMatrix& ContiguousMatrix::operator=(const ContiguousMatrix& other){
        if (width == other.width && height == other.height &&
            communicator.getProcessorNumber() == other.communicator.getProcessorNumber()){
            int nprocs = communicator.getProcessorNumber();
            localSize = ceil((double) size / nprocs);
            iStart = localSize * rank;
            iFinish = iStart + localSize;
            if (iFinish > size) {
                iFinish = size;
                localSize = iFinish - iStart;
            }
            data = bigData + iStart;
        } else {
            size = other.size;
            width = other.width;
            height = other.height;
            widthUm = other.widthUm;
            heightUm = other.heightUm;
            rank = communicator.getRank();
            int nprocs = communicator.getProcessorNumber();
            localSize = ceil((double) size / nprocs);
            iStart = localSize * rank;
            iFinish = iStart + localSize;
            if (iFinish > size) {
                iFinish = size;
                localSize = iFinish - iStart;
            }
            deleteBuffers();
            createBuffers();
        }
        memcpy(data, other.data, localSize * sizeof(double));
        return *this;
    }

    ContiguousMatrix& ContiguousMatrix::operator=(ContiguousMatrix&& other){
        if (communicator.getProcessorNumber() != other.communicator.getProcessorNumber()){
            throw matrix_move_error();
        }
        deleteBuffers();
        width = other.width;
        height = other.height;
        widthUm = other.widthUm;
        heightUm = other.heightUm;
        size = other.size;
        localSize = other.localSize;
        rank = other.rank;
        iStart = other.iStart;
        iFinish = other.iFinish;
        allocatorSize = other.allocatorSize;
        data = other.data;
        other.data = nullptr;
        bigData = other.bigData;
        other.bigData = nullptr;
        synchronizationBuffer = other.synchronizationBuffer;
        other.synchronizationBuffer = nullptr;
        return *this;
    }

    ContiguousMatrix ContiguousMatrix::operator-() const{
        ContiguousMatrix B(communicator, width, height, widthUm, heightUm);
        B.calculate(*this, [](Matrix::ConstantIterator& a){
            return -*a;
        });
        return B;
    }

    ContiguousMatrix& ContiguousMatrix::transpose(){
        Iterator b(*this, 0, 0);
        for (auto a = begin(); a != end(); ++a){
            int i = a.getRow();
            int j = a.getColumn();
            *a = b.val(j, i);
        }
        return *this;
    }

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

static const double INTERPOLATION_MATRIX[16][16] =
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

    ContiguousMatrix& ContiguousMatrix::interpolate(const data::ContiguousMatrix &A) {
        double Nx_float = 1.0 * getWidth() / A.getWidth();
        double Ny_float = 1.0 * getHeight() / A.getHeight();
        int Nx = (int)Nx_float;
        int Ny = (int)Ny_float;
        if (Nx != Nx_float || Ny != Ny_float || A.getWidth() == 1 || A.getHeight() == 1) {
            throw matrix_dimensions_mismatch();
        }

        int nprocs = communicator.getProcessorNumber();
        int rank = communicator.getRank();
        int total_rows = A.getHeight();
        int total_cols = A.getWidth();
        int local_rows = total_rows / nprocs + (total_rows % nprocs != 0);
        int rows_to_gather = local_rows * nprocs;
        int recvbuf_size = rows_to_gather * Ny * total_cols * Nx;
        int sendcount = local_rows * Ny * total_cols * Nx;
        int start_row = local_rows * rank;
        int finish_row = start_row + local_rows;
        if (finish_row > total_rows){
            finish_row = total_rows;
            local_rows = finish_row - start_row;
        }

        for (int i0 = start_row; i0 < finish_row; ++i0){
            for (int j0 = 0; j0 < A.getWidth(); ++j0){
                ConstantIterator F(A, i0, j0);
                double derivatives[16];

                /* Local copy of the source matrix is represented here. If some values from here are missing due
                 * to borders they will be guessed based on slope values. abbreviation 'm' means 'minus' */
                double          F_20, F_m10;
                double          F_21, F_m11;
                double F_02,    F_12, F_m12;
                double F_22, F_m1m1, F_0m1, F_1m1, F_2m1;
                F_00 = F.val(0, 0);
                if (i0 == total_rows - 1){ // # rows > 1 guarantees that i0 >= 1 => valid row and i0-1 >= 0 => valid row
                    F_10 = 2 * F.val(0, 0) - F.val(-1, 0);
                } else {
                    F_10 = F.val(1, 0);
                }
                if (i0 >= total_rows - 2){
                    F_20 = 2 * F_10 - F_00;
                } else {
                    F_20 =F.val(2, 0);
                }
                if (i0 == 0){
                    F_m10 = 2 * F_00 - F_10;
                } else {
                    F_m10 = F.val(-1, 0);
                }
                if (j0 == total_cols -1){ // # cols > 1 guarantees that j0 >= 1 => valid col, j0-1 >= 0 => valid col
                    F_01 = 2 * F.val(0, 0) - F.val(0, -1);
                } else {
                    F_01 = F.val(0, 1);
                }
                if (i0 == total_rows - 1 && j0 == total_cols - 1){ // the last item
                    F_11 = 2 * F.val(0, 0) - F.val(-1, -1);
                } else if (j0 == total_cols - 1){ // the last col but not row
                    F_11 = 2 * F.val(1, 0) - F.val(1, -1);
                } else if (i0 == total_rows - 1){ // the last row but not col
                    F_11 = 2 * F.val(0, 1) - F.val(-1, 1);
                } else {
                    F_11 = F.val(1, 1);
                }
                if (i0 >= total_rows - 2){
                    F_21 = 2 * F_11 - F_10;
                } else {
                    F_21 = F.val(2, 1);
                }
                if (i0 == 0){
                    F_m11 = 2 * F_01 - F_11;
                } else {
                    F_m11 = F.val(-1, 1);
                }
                if (j0 >= total_cols - 2){
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

                double a[4][4];
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


                Iterator G(*this, i0 * Ny, j0 * Nx);
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
        }

        double* recvbuf = new double[recvbuf_size];
        Iterator G0(*this, start_row * Ny, 0);
        double* sendbuf = &(*G0);
        communicator.allGather(sendbuf, sendcount, MPI_DOUBLE, recvbuf, sendcount, MPI_DOUBLE);
        Iterator G(*this, 0);
        for (int i=0; i < size; ++i, ++G){
            *G = recvbuf[i];
        }

        delete [] recvbuf;

        return *this;
    }

#if DEBUG==1
    ContiguousMatrix ContiguousMatrix::magic(mpi::Communicator& comm, int n){
        if (n % 2 == 0){
            throw std::runtime_error("The method was not implemented for even matrix sizes");
        }

        ContiguousMatrix A(comm, n, n, 1.0, 1.0);
        Iterator a(A, 0, 0);
        int i = n/2;
        int j = n-1;
        for (double value = 1; value <= n*n; ++value){
            a.val(i, j) = value;

            /* First rule: */
            --i, ++j;

            /* Third rule: */
            if (i == -1 && j == n){
                i = 0, j = n-2;
            }

            /* Ensure that all indices are valid */
            if (i<0) i = n-1;
            if (j>=n) j = 0;

            /* Second rule */
            if (a.val(i, j) != 0.0){
                ++i, j-=2;
            }

            /* Ensure again that all indices are valid */
            if (i>=n) i = 0;
            if (j<0) j += n;
        }
        return A;
    }
#endif


}