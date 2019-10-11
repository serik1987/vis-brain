//
// Created by serik1987 on 21.09.19.
//

#include "data/LocalMatrix.h"

void test_main(){
    using namespace std;

    logging::progress(0, 1, "Creating matrix with Gaussian shape");

    mpi::Communicator& comm = Application::getInstance().getAppCommunicator();
    data::LocalMatrix A(comm, 5, 5, 1.0, 1.0);
    const double sigma = 0.5;

    for (auto a = A.begin(); a != A.end(); ++a){
        double x = a.getColumnUm();
        double y = a.getRowUm();
        *a = exp(-(x*x+y*y)/(sigma*sigma));
    }

    logging::enter();
    A.printLocal();
    logging::exit();

    logging::progress(0, 1, "Calculating matrix sum");
    double sum = A.sum();
    logging::enter();
    logging::debug("Matrix sum: " + std::to_string(sum));
    logging::exit();

    logging::progress(0, 1, "Matrix normalization");
    for (auto a = A.begin(); a != A.end(); ++a){
        *a /= sum;
    }
    sum = A.sum();
    logging::enter();
    logging::debug("Matrix A, after normalization");
    A.printLocal();
    logging::debug("Matrix sum, after normalization: " + std::to_string(sum));
    logging::exit();

    logging::progress(0, 1, "Maximum value test");
    double max = A.max();
    logging::enter();
    logging::debug("Maximum value: " + std::to_string(max));
    logging::exit();

    logging::progress(0, 1, "Reverse matrix");
    data::LocalMatrix B(comm, 5, 5, 1.0, 1.0);
    B.calculate(A, [](auto x){
       return -*x;
    });
    double sum2 = B.sum();
    double max2 = B.max();
    logging::enter();
    logging::debug("Matrix B content");
    B.printLocal();
    logging::debug("Matrix B sum: " + std::to_string(sum2));
    logging::debug("Matrix B max: " + std::to_string(max2));
    logging::exit();

    logging::progress(0, 1, "argmax test for matrix A");
    int row, col, index;
    index = A.argmax(row, col);
    logging::enter();
    logging::debug("Index of the maximum value: " + std::to_string(index));
    logging::debug("Row of the maximum value: " + std::to_string(row));
    logging::debug("Column of the maximum value: " + std::to_string(col));
    logging::exit();

    logging::progress(0, 1, "argmax test for matrix B");
    index = B.argmax(row, col);
    logging::enter();
    logging::debug("Index of the maximum value: " + std::to_string(index));
    logging::debug("Row of the maximum value: " + std::to_string(row));
    logging::debug("Column of the maximum value: " + std::to_string(col));
    logging::exit();

    logging::progress(0, 1, "Test for min values");
    double min = A.min();
    double min2 = B.min();
    logging::enter();
    logging::debug("Minimum value for A: " + std::to_string(min));
    logging::debug("Minimum value for B: " + std::to_string(min2));
    logging::exit();

    logging::progress(0, 1, "Test for argmin");
    index = A.argmin(row, col);
    logging::enter();
    logging::debug("Matrix A. Index of the minimum value: " + std::to_string(index));
    logging::debug("Matrix A. Row of the minimum value: " + std::to_string(row));
    logging::debug("Matrix A. Column of the minimum value: " + std::to_string(col));
    logging::exit();
    index = B.argmin(row, col);
    logging::enter();
    logging::debug("Matrix B. Index of the minimum value: " + std::to_string(index));
    logging::debug("Matrix B. Row of the minimum value: " + std::to_string(row));
    logging::debug("Matrix B. Column of the minimum value: " + std::to_string(col));
    logging::exit();

    logging::progress(0, 1, "Mean value test");
    double mean = A.mean();
    double mean2 = B.mean();
    logging::enter();
    logging::debug("Matrix A. Mean value: " + std::to_string(mean));
    logging::debug("Matrix B. Mean value: " + std::to_string(mean2));
    logging::exit();

    logging::progress(0, 1, "Standard deviation test");
    double std = A.std();
    double std2 = B.std();
    logging::enter();
    logging::debug("Matrix A. Standard deviation: " + std::to_string(std));
    logging::debug("Matrix B. Standard deviation: " + std::to_string(std2));
    logging::exit();

    logging::progress(0, 1, "Standard error of mean test");
    double sterr = A.sterr();
    double sterr2 = B.sterr();
    logging::enter();
    logging::debug("Matrix A. Standard error of mean: " + std::to_string(sterr));
    logging::debug("Matrix B. Standard error of mean: " + std::to_string(sterr2));
    logging::exit();

    logging::progress(0, 1, "Error function");
    double err = A.errorfunc(B);
    logging::enter();
    logging::debug("Error function: " + std::to_string(err));
    logging::exit();

    logging::progress(0, 1, "Covariance");
    double cov = A.cov(B);
    logging::enter();
    logging::debug("Matrix covariance: " + std::to_string(cov));
    logging::exit();

    logging::progress(0, 1, "Correlation");
    double corr = A.corr(B);
    logging::enter();
    logging::debug("Matrix correlation: " + std::to_string(corr));
    logging::exit();

    logging::progress(1, 1);
}