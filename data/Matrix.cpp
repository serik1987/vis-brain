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