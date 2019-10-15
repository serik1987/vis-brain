//
// (C) Sergei Kozhukhov, 2019
// (C) the Institute of Higher Nervous Activity and Neurophysiology, Russian Academy of Sciences, 2019
//

#include "Matrix.h"
#include "../Application.h"
#include "ContiguousMatrix.h"

namespace data{

    Matrix::Matrix(mpi::Communicator &comm, int w, int h, double w_um, double h_um, double filler): communicator(comm),
    width(w), height(h), widthUm(w_um), heightUm(h_um)
    {
        size = width * height;
        rank = comm.getRank();
        localSize = ceil((double)size/comm.getProcessorNumber());
        iStart = localSize * rank;
        iFinish = iStart + localSize;
        if (iFinish > size){
            iFinish = size;
            localSize = iFinish - iStart;
        }
    }

#if DEBUG==1
    void Matrix::printLocal() const{
        using namespace std;

        for (int i=0; i < height; i++){
            ostringstream ss;
            ss << "\t";
            for (int j=0; j < width; j++){
                try {
                    ss << getValue(i, j) << "\t";
                } catch (std::exception& e){
                    ss << "XXXXX\t";
                }
            }
            Application::getInstance().getLoggingEngine().debug(ss.str());
        }
        Application::getInstance().getLoggingEngine().debug("");
    }
#endif



    void Matrix::fill(double x){
        for (auto a = begin(); a != end(); ++a){
            *a = x;
        }
    }

    Matrix& Matrix::operator++(){
        for (auto a = begin(); a != end(); ++a){
            ++*a;
        }
        return *this;
    }

    Matrix& Matrix::operator--(){
        for (auto a = begin(); a != end(); ++a){
            --*a;
        }
        return *this;
    }

    Matrix& Matrix::operator+=(const Matrix& other){
        auto a = begin();
        auto b = other.cbegin();
        for (; a != end(); ++a, ++b){
            *a += *b;
        }
        return *this;
    }

    Matrix& Matrix::operator-=(const Matrix& other){
        auto a = begin();
        auto b = other.cbegin();
        for (; a != end(); ++a, ++b){
            *a -= *b;
        }
        return *this;
    }

    Matrix& Matrix::operator+=(double x){
        for (auto a = begin(); a != end(); ++a){
            *a += x;
        }
        return *this;
    }

    Matrix& Matrix::operator-=(double x){
        for (auto a = begin(); a != end(); ++a){
            *a -= x;
        }
        return *this;
    }

    Matrix& Matrix::add(const Matrix& A, const Matrix& B){
        calculate(A, B, [](ConstantIterator& a, ConstantIterator& b){
            return *a + *b;
        });
        return *this;
    }

    Matrix& Matrix::sub(const Matrix& A, const Matrix& B){
        calculate(A, B, [](ConstantIterator& a, ConstantIterator& b){
            return *a - *b;
        });
        return *this;
    }

    Matrix& Matrix::add(const Matrix& A, double x){
        calculate(A, [&x](ConstantIterator& a){
            return *a + x;
        });
        return *this;
    }

    Matrix& Matrix::sub(const Matrix& A, double x){
        calculate(A, [&x](ConstantIterator& a){
            return *a - x;
        });
        return *this;
    }


    Matrix& Matrix::sub(double x, const Matrix& A){
        calculate(A, [&x](ConstantIterator& a){
            return x - *a;
        });
        return *this;
    }

    Matrix& Matrix::neg(const Matrix& A){
        calculate(A, [](ConstantIterator& a){
            return -*a;
        });
        return *this;
    }

    Matrix& Matrix::operator*=(const Matrix& other){
        auto a = begin();
        auto b = other.cbegin();
        for (; a != end(); ++a, ++b){
            *a *= *b;
        }
        return *this;
    }

    Matrix& Matrix::operator/=(const Matrix& other){
        auto a = begin();
        auto b = other.cbegin();
        for (; a != end(); ++a, ++b){
            *a /= *b;
        }
        return *this;
    }

    Matrix& Matrix::operator*=(double x){
        for (auto a = begin(); a != end(); ++a){
            *a *= x;
        }
        return *this;
    }

    Matrix& Matrix::operator/=(double x){
        for (auto a = begin(); a != end(); ++a){
            *a /= x;
        }
        return *this;
    }

    Matrix& Matrix::mul(const Matrix& A, const Matrix& B){
        calculate(A, B, [](ConstantIterator& a, ConstantIterator& b){
            return *a * *b;
        });
        return *this;
    }

    Matrix& Matrix::div(const Matrix& A, const Matrix& B){
        calculate(A, B, [](ConstantIterator& a, ConstantIterator& b){
            return *a / *b;
        });
        return *this;
    }

    Matrix& Matrix::mul(const Matrix& A, double x){
        calculate(A, [&x](ConstantIterator& a){
            return *a * x;
        });
        return *this;
    }

    Matrix& Matrix::div(const Matrix& A, double x){
        calculate(A, [&x](ConstantIterator& a){
            return *a / x;
        });
        return *this;
    }

    Matrix& Matrix::div(double x, const Matrix& A){
        calculate(A, [&x](ConstantIterator& a){
            return x / *a;
        });
        return *this;
    }

    Matrix& Matrix::transpose(const ContiguousMatrix& A){
        data::ContiguousMatrix::ConstantIterator a(A, 0);
        for (auto b = begin(); b != end(); ++b){
            int i = b.getRow();
            int j = b.getColumn();
            *b = a.val(j, i);
        }
    }

    Matrix& Matrix::dot(const ContiguousMatrix& A, const ContiguousMatrix& B){
        data::ContiguousMatrix::ConstantIterator a(A, 0);
        data::ContiguousMatrix::ConstantIterator b(B, 0);
        for (auto c = begin(); c != end(); ++c){
            int i = c.getRow();
            int j = c.getColumn();
            *c = 0.0;
            for (int k=0; k < A.getWidth(); ++k){
                *c += a.val(i, k) * b.val(k, j);
            }
        }
        return *this;
    }

    double Matrix::sum() const{
        double r = 0.0;
        r = reduce(0.0, MPI_SUM, [](double& result, double x){
            result += x;
        });
        return r;
    }

    double Matrix::max() const{
        double m = 0.0;
        bool initialized = false;
        m = reduce(NAN, MPI_MAX, [&initialized](double& result, double x){
            if (initialized){
                if (x > result) result = x;
            } else {
                result = x;
                initialized = true;
            }
        });
        return m;
    }

    struct MPI_MAX_MIN_LOC_STRUCTURE {
        double value;
        int rank;
    };

    int Matrix::argmax(int &row, int &col, double *value) const {
        auto a = cbegin();
        double localValue = *a;
        int localIndex = a.getIndex();
        int localRow = a.getRow();
        int localCol = a.getColumn();
        double globalValue;

        for (++a; a != cend(); ++a){
            if (*a > localValue){
                localValue = *a;
                localIndex = a.getIndex();
                localRow = a.getRow();
                localCol = a.getColumn();
            }
        }

        MPI_MAX_MIN_LOC_STRUCTURE local, global;
        local.value = localValue;
        local.rank = communicator.getRank();
        communicator.allReduce(&local, &global, 1, MPI_DOUBLE_INT, MPI_MAXLOC);
        if (value != nullptr) *value = global.value;

        int indices[3] = {localIndex, localRow, localCol};
        communicator.broadcast(indices, 3, MPI_INT, global.rank);
        row = indices[1];
        col = indices[2];
        return indices[0];
    }

    double Matrix::min() const{
        double minValue;
        bool initialized = false;
        minValue = reduce(NAN, MPI_MIN, [&initialized](double& result, double x){
            if (initialized){
                if (x < result) result = x;
            } else {
                result = x;
                initialized = true;
            }
        });
        return minValue;
    }

    int Matrix::argmin(int &row, int &col, double *value) const {
        ConstantIterator a = cbegin();
        double localValue = *a;
        int localIndex = a.getIndex();
        int localRow = a.getRow();
        int localCol = a.getColumn();

        for (++a; a != cend(); ++a){
            if (*a < localValue){
                localValue = *a;
                localIndex = a.getIndex();
                localRow = a.getRow();
                localCol = a.getColumn();
            }
        }

        MPI_MAX_MIN_LOC_STRUCTURE localStructure, globalStructure;
        localStructure.value = localValue;
        localStructure.rank = communicator.getRank();
        communicator.allReduce(&localStructure, &globalStructure, 1, MPI_DOUBLE_INT, MPI_MINLOC);
        if (value != nullptr) *value = globalStructure.value;
        int root = globalStructure.rank;

        int indices[3] = {localIndex, localRow, localCol};
        communicator.broadcast(indices, 3, MPI_INT, root);
        row = indices[1];
        col = indices[2];
        return indices[0];
    }

#define SQR(X) ((X)*(X))

    double Matrix::std() const{
        const int VALUES = 0;
        const int SQUARES = 1;
        double localSum[2] = {0, 0};
        double globalSum[2] = {0, 0};
        double deviation;
        int items = height * width;

        for (auto a = cbegin(); a != cend(); ++a){
            localSum[VALUES] += *a;
            localSum[SQUARES] += *a * *a;
        }
        communicator.allReduce(localSum, globalSum, 2, MPI_DOUBLE, MPI_SUM);
        deviation = sqrt(globalSum[SQUARES]/items - SQR(globalSum[VALUES]/items));

        return deviation;
    }

    double Matrix::errorfunc(const data::Matrix &other) const {
        double localError, globalError;

        localError = 0.0;
        for (auto a = cbegin(), b = other.cbegin(); a != cend(); ++a, ++b){
            localError += SQR(*a - *b);
        }
        communicator.allReduce(&localError, &globalError, 1, MPI_DOUBLE, MPI_SUM);

        return globalError/2.0;
    }

    double Matrix::cov(const data::Matrix &other) const {
        const int MUTUAL_SUM = 0;
        const int FIRST_SUM = 1;
        const int SECOND_SUM = 2;
        double N = width * height;
        double local[3] = {0.0, 0.0, 0.0};
        double global[3];
        double cov;

        for (auto a = cbegin(), b = other.cbegin(); a != cend(); ++a, ++b){
            local[MUTUAL_SUM] += *a * *b;
            local[FIRST_SUM] += *a;
            local[SECOND_SUM] += *b;
        }
        communicator.allReduce(local, global, 3, MPI_DOUBLE, MPI_SUM);
        for (int i=0; i < 3; ++i) global[i] /= N;
        cov = global[MUTUAL_SUM] - global[FIRST_SUM] * global[SECOND_SUM];

        return cov;
    }

    double Matrix::corr(const data::Matrix& other) const {
        const int MUTUAL_SUM = 0;
        const int A_SUM = 1;
        const int B_SUM = 2;
        const int SQR_A_SUM = 3;
        const int SQR_B_SUM = 4;
        double local[5] = {0.0, 0.0, 0.0, 0.0, 0.0};
        double global[5];
        int N = width * height;
        double covAB;
        double varA;
        double varB;
        double corrAB;

        for (auto a = cbegin(), b = other.cbegin(); a != cend(); ++a, ++b){
            local[MUTUAL_SUM] += *a * *b;
            local[A_SUM] += *a;
            local[B_SUM] += *b;
            local[SQR_A_SUM] += SQR(*a);
            local[SQR_B_SUM] += SQR(*b);
        }
        communicator.allReduce(local, global, 5, MPI_DOUBLE, MPI_SUM);
        for (int i=0; i < 5; ++i) global[i] /= N;
        covAB = global[MUTUAL_SUM] - global[A_SUM] * global[B_SUM];
        varA = global[SQR_A_SUM] - SQR(global[A_SUM]);
        varB = global[SQR_B_SUM] - SQR(global[B_SUM]);
        corrAB = covAB / sqrt(varA * varB);

        return corrAB;
    }


    Matrix& Matrix::convolve(const data::ContiguousMatrix &K, const data::ContiguousMatrix &A) {
        if (A.getWidth() != width || A.getHeight() != height){
            throw matrix_dimensions_mismatch();
        }

        Matrix::Iterator b = begin();
        Matrix::ConstantIterator a = A.cbegin();
        int W = (K.getWidth() - 1)/2;
        int H = (K.getHeight() - 1)/2;
        ContiguousMatrix::ConstantIterator k(K, W, H);

        for (; b != end(); ++b, ++a){
            *b = 0.0;
            double local_sum = 0.0;
            for (int h = -H; h <= H; ++h){
                int i_loc = a.getRow() + h;
                if (i_loc >= 0 && i_loc < A.getHeight()){
                    for (int w = -W; w < W; ++w){
                        int j_loc = a.getColumn() + w;
                        if (j_loc >= 0 && j_loc < A.getHeight()){
                            *b += k.val(h, w) * a.val(h, w);
                            local_sum += k.val(h, w);
                        }
                    }
                }
            }
            *b /= local_sum;
        }

        return *this;
    }


    Matrix& Matrix::downsample(const ContiguousMatrix& source){
        int sourceHeight = source.getHeight();
        int sourceWidth = source.getWidth();
        double Ny_float = (double)sourceHeight/height;
        double Nx_float = (double)sourceWidth/width;
        int Ny = (int)Ny_float;
        int Nx = (int)Nx_float;
        if (Nx != Nx_float || Ny != Ny_float){
            throw matrix_dimensions_mismatch();
        }

        for (auto b = begin(); b != end(); ++b){
            int i0 = b.getRow();
            int j0 = b.getColumn();
            ContiguousMatrix::ConstantIterator a(source, i0 * Ny, j0 * Nx);
            *b = 0;
            for (int i = 0; i < Nx; ++i){
                for (int j = 0; j < Ny; ++j){
                    *b += a.val(i, j);
                }
            }
        }
    }


}

void swap(data::Matrix& A, data::Matrix& B){
    {
        for (auto a = A.begin(), b = B.begin(); a != A.end(); ++a, ++b){
            double temp = *a;
            *a = *b;
            *b = temp;
        }
    }
}