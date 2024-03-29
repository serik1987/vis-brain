//
// (C) Sergei Kozhukhov, 2019
// (C) the Institute of Higher Nervous Activity and Neurophysiology, Russian Academy of Sciences, 2019
//

#include <cstring>
#include "LocalMatrix.h"
#include "../Application.h"


namespace data{

    LocalMatrix::LocalMatrix(mpi::Communicator &comm, int width, int height, double widthUm, double heightUm,
                             double filler): Matrix(comm, width, height, widthUm, heightUm, filler) {
        data = new double[localSize];
        for (int i=0; i < localSize; i++){
            data[i] = filler;
        }
    }

    LocalMatrix::LocalMatrix(const LocalMatrix &other):
        Matrix(other.communicator, other.width, other.height, other.widthUm, other.heightUm) {
        copyData(other);
    }

    LocalMatrix& LocalMatrix::operator=(const LocalMatrix &other) {
        size = other.size;
        localSize = other.localSize;
        iStart = other.iStart;
        iFinish = other.iFinish;
        width = other.width;
        height = other.height;
        widthUm = other.widthUm;
        heightUm = other.heightUm;
        if (width != other.width || height != other.height) {
            delete[] data;
            copyData(other);
        } else {
            memcpy(data, other.data, localSize * sizeof(double));
        }
        return *this;
    }

    LocalMatrix& LocalMatrix::operator=(LocalMatrix&& other) noexcept{
        size = other.size;
        localSize = other.localSize;
        iStart = other.iStart;
        iFinish = other.iFinish;
        width = other.width;
        height = other.height;
        widthUm = other.widthUm;
        heightUm = other.heightUm;
        delete [] data;
        data = other.data;
        other.data = nullptr;
        return *this;
    }

    void LocalMatrix::copyData(const LocalMatrix& other){
        data = new double[localSize];
        std::memcpy(data, other.data, localSize*sizeof(double));
    }

    LocalMatrix::~LocalMatrix() {
        if (data != nullptr) {
            delete[] data;
        }
    }

    LocalMatrix LocalMatrix::operator-() const{
        LocalMatrix B(communicator, width, height, widthUm, heightUm);
        B.calculate(*this, [](Matrix::ConstantIterator& a){
            return -*a;
        });
        return B;
    }



}