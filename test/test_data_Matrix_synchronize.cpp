//
// Created by serik1987 on 21.09.19.
//

#include "../Application.h"
#include "../data/Matrix.h"
#include "../data/LocalMatrix.h"
#include "../data/ContiguousMatrix.h"

void print_matrix(data::Matrix& matrix, const std::string& prompt){
    using std::to_string;

    logging::enter();
    logging::debug(prompt);
    matrix.printLocal();
    logging::debug("");
    logging::exit();
}

void fill_matrix(data::Matrix& matrix){
    for (int i=0; i < matrix.getHeight(); i++){
        for (int j=0; j < matrix.getWidth(); j++){
            int index = i * matrix.getWidth() + j;
            if (index >= matrix.getIstart() && index < matrix.getIfinish()){
                matrix.getValue(i, j) = 100*i + j;
            }
        }
    }
}

void test_matrix(data::Matrix& matrix){
    try {
        fill_matrix(matrix);
        print_matrix(matrix, "Matrix before synchronization process");
        matrix.synchronize(3);
        print_matrix(matrix, "Matrix after syncrhonization process for rank = 3");
        matrix.synchronize();
        print_matrix(matrix, "Matrix after synchronization process for all ranks");
    } catch (simulation_exception& e){
        logging::error(e);
    }
}

void test_main(){
    using namespace std;

    Application& app = Application::getInstance();
    mpi::Communicator& comm = app.getAppCommunicator();
    data::LocalMatrix local_matrix(comm, 20, 20, 1.0, 1.0);
    data::ContiguousMatrix contiguous_matrix(comm, 20, 20, 1.0, 1.0);
    logging::progress(0, 2, "Local matrix test");
    test_matrix(local_matrix);
    logging::progress(1, 2, "Contiguous matrix test");
    test_matrix(contiguous_matrix);
    logging::progress(2, 2);
}