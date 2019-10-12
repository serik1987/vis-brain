//
// Created by serik1987 on 21.09.19.
//

#include "data/LocalMatrix.h"
#include "data/ContiguousMatrix.h"
#include "data/reader/BinReader.h"
#include "sys/FileInfo.h"

void test_main(){
    using namespace std;

    logging::progress(0, 1, "Generating matrices");

    mpi::Communicator& comm = Application::getInstance().getAppCommunicator();
    data::LocalMatrix A(comm, 20, 20, 1.0, 1.0);
    data::ContiguousMatrix B = data::ContiguousMatrix::magic(comm, 11);
    const double sigma = 0.5;

    for (auto a = A.begin(); a != A.end(); ++a){
        double x = a.getColumnUm();
        double y = a.getRowUm();
        *a = exp(-(x*x + y*y)/(2*sigma*sigma));
    }
    double sumA = A.sum();
    for (auto a = A.begin(); a != A.end(); ++a){
        *a /= sumA;
    }

    logging::progress(0, 1, "Writing matrices test");
    data::reader::Saver* reader;
    {
        using namespace data::reader;
        reader = dynamic_cast<Saver*>(Reader::createReader("bin", "matrix-A.bin"));
        if (reader == nullptr){
            throw std::runtime_error("The reader doesn't support saving");
        }
    }
    reader->save(A);

    logging::enter();
    logging::debug("Information about matrix A");
    logging::debug("Filename: " + reader->getFilename());
    logging::exit();

    reader->save(B, "matrix-B.bin");
    logging::enter();
    logging::debug("Matrix B was also successfully saved");
    logging::exit();

    logging::progress(0, 1, "Check for file existence");
    sys::FileInfo info1("matrix-A-python.bin"), info2("matrix-B-python.bin");

    logging::progress(0, 1, "Loading matrix B");
    data::Matrix *A2 = nullptr, *B2 = nullptr;
    reader->setFilename("matrix-B-python.bin");
    data::reader::Loader* new_reader = dynamic_cast<data::reader::Loader*>(reader);
    B2 = new_reader->load(comm);

    logging::progress(0, 1, "Matrix loading check");
    logging::enter();
    logging::debug("Matrix B");
    B2->printLocal();
    for (auto b = B.cbegin(), b2 = B2->cbegin(); b != B.cend(); ++b, ++b2){
        double diff = abs(*b - *b2);
        if (diff > 1e-10){
            logging::debug("\033[31mLoading check failed:\033[0m b = " + std::to_string(*b) + "; b2 = " + std::to_string(*b2));
        }
    }
    logging::exit();

    logging::progress(0, 1, "Loading matrix A");
    A2 = new_reader->load(comm, "matrix-A-python.bin");

    logging::progress(0, 1, "Matrix A loading check");
    logging::enter();
    for (auto a = A.cbegin(), a2 = A2->cbegin(); a != A.cend(); ++a, ++a2){
        double diff = abs(*a - *a2);
        if (diff > 1e-10){
            logging::debug("\033[31mLoading check failed:\033[0m b = " + std::to_string(*a) + "; b2 = " + std::to_string(*a2));
        }
    }
    logging::exit();

    delete reader;
    delete A2;
    delete B2;
    logging::progress(1, 1);
}