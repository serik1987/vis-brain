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

    logging::progress(0, 1, "Magic matrix: calculating");

    const int n = 2001;
    double sum = 0;
    mpi::Communicator& comm = Application::getInstance().getAppCommunicator();
    data::ContiguousMatrix A = data::ContiguousMatrix::magic(comm, n);

    {
        logging::progress(0, 1, "Magic matrix: column check");
        data::ContiguousMatrix::Iterator a(A, 0);
        for (int i = 0; i < n; ++i) {
            double local_sum = 0;
            for (int j = 0; j < n; ++j) {
                local_sum += a.val(i, j);
                if (a.val(i, j) == 0.0) {
                    throw std::runtime_error("Value is not filled");
                }
            }
            if (sum != local_sum && sum != 0) {
                throw std::runtime_error("Column sum inconsistency");
            }
            sum = local_sum;
        }

        logging::progress(0, 1, "Magic matrix: row check");

        for (int j = 0; j < n; ++j) {
            double local_sum = 0;
            for (int i = 0; i < n; ++i) {
                local_sum += a.val(i, j);
            }
            if (sum != local_sum) {
                throw std::runtime_error("Row sum inconsistency");
            }
        }

        logging::progress(0, 1, "check diagonals");

        double local_sum = 0;
        for (int i = 0, j = 0; i < n; ++i, ++j) {
            local_sum += a.val(i, j);
        }
        if (sum != local_sum) {
            throw std::runtime_error("Main diagonal inconsistency");
        }

        local_sum = 0;
        for (int i = 0, j = n - 1; i < n; ++i, --j) {
            local_sum += a.val(i, j);
        }
    }

    logging::enter();
    if (comm.getRank() == 0) {
        logging::debug("Matrix A");
        logging::debug("Magic sum: " + to_string(sum));
        logging::debug("n = " + to_string(n));
    }
    logging::exit();

    data::LuDecomposer decomposer(A, "LU decomposition", 50);
    decomposer.printDebugInformation();

    logging::progress(0, 1, "Checking decomnposed information");
    for (int i=0; i < n; ++i){
        for (int j = 0; j < n; ++j){
            double PA_real = decomposer.PA(i, j);
            double PA_decomposed = 0.0;
            for (int k = 0; k < n; ++k){
                PA_decomposed += decomposer.L(i, k) * decomposer.U(k, j);
            }
            if (abs(PA_real - PA_decomposed) > 1e-8) {
                logging::enter();
                if (comm.getRank() == 0) {
                    logging::debug("i = " + std::to_string(i) + "; j = "
                                   + std::to_string(j) + "; PA_real = " + std::to_string(PA_real) +
                                   "; PA_decomposed = " + std::to_string(PA_decomposed));
                }
                logging::exit();
                throw std::runtime_error("Decomposition test failed");
            }
        }
    }



    logging::progress(1, 1);
}