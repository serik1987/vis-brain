//
// Created by serik1987 on 21.09.19.
//

#include "data/LocalMatrix.h"
#include "data/ContiguousMatrix.h"
#include "data/reader/BinReader.h"

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


    logging::progress(1, 1);
}