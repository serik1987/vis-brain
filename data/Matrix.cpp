//
// Created by serik1987 on 21.09.19.
//

#include "Matrix.h"
#include "../Application.h"

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