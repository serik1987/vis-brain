//
// Created by serik1987 on 21.09.19.
//

#include "Application.h"
#include "data/Matrix.h"
#include "data/LocalMatrix.h"
#include "data/ContiguousMatrix.h"
#include "data/LuDecomposer.h"

void test_main(){
    using namespace std;

    logging::progress(0, 1, "Calculating magic matrix");

    const int n = 7;
    double sum = 0;
    mpi::Communicator& comm = Application::getInstance().getAppCommunicator();
    data::ContiguousMatrix A = data::ContiguousMatrix::magic(comm, n);
    logging::enter();
    A.printLocal();
    logging::exit();

    logging::progress(0, 1, "LU decomposition");
    data::LuDecomposer decomposer(A);
    data::ContiguousMatrix L = decomposer.getLowerTriangle();
    data::ContiguousMatrix U = decomposer.getUpperTriangle();
    data::ContiguousMatrix P = decomposer.getPermutationMatrix();
    logging::enter();
    logging::debug("Lower triangle matrix");
    L.printLocal();
    logging::debug("Upper triangle matrix");
    U.printLocal();
    logging::debug("Permutation matrix");
    P.printLocal();
    logging::exit();

    logging::progress(0, 1, "LU decomposition test");
    data::ContiguousMatrix left_side(comm, n, n, 1.0, 1.0);
    data::ContiguousMatrix right_side(comm, n, n, 1.0, 1.0);
    data::ContiguousMatrix equality(comm, n, n, 1.0, 1.0);
    left_side.dot(L, U);
    right_side.dot(P, A);
    equality.sub(left_side, right_side);
    logging::enter();
    logging::debug("L*U value");
    left_side.printLocal();
    logging::debug("P*A value");
    right_side.printLocal();
    logging::debug("Control check");
    equality.printLocal();
    logging::exit();

    for (auto e: equality){
        if (abs(e) > 1e-10){
            throw std::runtime_error("LU decomposition test failed");
        }
    }

    logging::progress(0, 1, "Solving A*x = b");

    data::ContiguousMatrix b(comm, 1, n, 1.0, 1.0/n);
    data::ContiguousMatrix x(comm, 1, n, 1.0, 1.0/n);
    for (int i=0; i < n; ++i){
        b[i] = (double)i;
    }

    logging::enter();
    logging::debug("b values");
    b.printLocal();
    logging::exit();

    logging::enter();
    decomposer.solve(x, b);
    logging::exit();

    logging::enter();
    logging::debug("x values");
    x.printLocal();
    logging::exit();

    logging::progress(0, 1, "Solution check");
    logging::enter();
    if (comm.getRank() == 0){
        logging::debug("Control values:");
        for (int i=0; i < n; ++i) {
            double check_value = 0.0;
            for (int j = 0; j < n; ++j) {
                check_value += A.getValue(i, j) * x.getValue(j);
            }
            check_value -= b.getValue(i);
            logging::debug(to_string(check_value));
            if (abs(check_value) > 1e-10){
                throw std::runtime_error("Solution check failed");
            }
        }
    }
    logging::debug("Solution check completed successfully");
    logging::exit();

    data::ContiguousMatrix B(A);
    B.synchronize();
    data::ContiguousMatrix X(comm, n, n, 1.0, 1.0);

    logging::progress(0, 1, "Matrix division");
    decomposer.divide(X, B);
    logging::enter();
    logging::debug("Division result:");
    X.printLocal();
    logging::exit();

    logging::progress(0, 1, "Matrix inverse");
    decomposer.inverse(X);
    logging::enter();
    logging::debug("Inverse result:");
    X.printLocal();
    logging::exit();

    logging::progress(0, 1, "Inverse matrix check");
    for (int i=0; i < n; ++i){
        for (int j=0; j < n; ++j){
            double control = 0.0;
            for (int k=0; k < n; ++k){
                control += A.getValue(i, k) * X.getValue(k, j);
            }
            control -= (double)(i == j);
            if (abs(control) > 1e-10){
                logging::enter();
                logging::debug("Inverse matrix check failure at (" + std::to_string(i)+", "+std::to_string(j) + "):");
                logging::debug("a = " + std::to_string(A.getValue(i, j)));
                logging::debug("x = " + std::to_string(X.getValue(i, j)));
                logging::debug("A*X = " + std::to_string(control));
                logging::debug("required: " + std::to_string((int)(i == j)));
                logging::exit();
                throw std::runtime_error("Inverse matrix check failed");
            }
        }
    }


    logging::progress(1, 1);
}