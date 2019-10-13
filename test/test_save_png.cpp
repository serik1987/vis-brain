//
// Created by serik1987 on 21.09.19.
//

#include "data/LocalMatrix.h"
#include "data/ContiguousMatrix.h"
#include "data/reader/Reader.h"
#include "sys/FileInfo.h"
#include "data/reader/Saver.h"
#include "data/reader/PngReader.h"

void print_reader_info(data::reader::Reader* reader){
    data::reader::PngReader* local_reader = dynamic_cast<data::reader::PngReader*>(reader);
    logging::enter();
    if (local_reader == nullptr){
        logging::debug("given reader is not a PNG reader");
    } else {
        logging::debug("Root process: " + std::to_string(local_reader->getRoot()));
        switch (local_reader->getColormap()){
            case data::reader::PngReader::GrayColormap:
                logging::debug("Colormap: gray");
                break;
            case data::reader::PngReader::JetColormap:
                logging::debug("Colormap: jet");
                break;
            case data::reader::PngReader::HsvColormap:
                logging::debug("Colormap: hsv");
                break;
            default:
                logging::debug("Unknown or unsupported colormap");
        }
        logging::debug("Minimum value: " + std::to_string(local_reader->getMinValue()));
        logging::debug("Maximum value: " + std::to_string(local_reader->getMaxValue()));
    }
    logging::exit();
}

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
        *a = exp(-(x*x + y*y)/(sigma*sigma));
    }
    double sumA = A.sum();
    for (auto a = A.begin(); a != A.end(); ++a){
        *a /= sumA;
    }

    double minA = A.min();
    double maxA = A.max();
    double minB = B.min();
    double maxB = B.max();

    logging::progress(0, 1, "Saving matrix A");
    data::reader::Saver* reader;
    reader = dynamic_cast<data::reader::Saver*>(data::reader::Reader::createReader("png", "matrix-A.png"));
    data::reader::PngReader* png_reader = dynamic_cast<data::reader::PngReader*>(reader);
    if (reader == nullptr){
        throw std::runtime_error("The reader doesn't support matrix saving");
    }
    png_reader->setColorRange(0, maxA);
    print_reader_info(reader);
    reader->save(A);
    logging::progress(0, 1, "Matrix A save, color mode");
    png_reader->setColormap(data::reader::PngReader::JetColormap);
    print_reader_info(reader);
    reader->save(A, "matrix-A-jet.png");
    logging::progress(0, 1, "Matrix B save, grayscale");
    png_reader->setColormap(data::reader::PngReader::GrayColormap);
    png_reader->setColorRange(minB, maxB);
    print_reader_info(reader);
    reader->save(B, "matrix-B.png");
    logging::progress(0, 1, "Matrix B save, color");
    png_reader->setColormap(data::reader::PngReader::HsvColormap);
    print_reader_info(reader);
    reader->save(B, "matrix-B-hsv.png");

    delete reader;
    logging::progress(1, 1);
}