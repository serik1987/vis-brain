//
// Created by serik1987 on 21.09.19.
//

#include "data/ContiguousMatrix.h"
#include "data/stream/BinStream.h"
#include "data/LocalMatrix.h"
#include "data/reader/PngReader.h"

void print_stream_info(data::stream::Stream& stream, const std::string& prompt){
    logging::enter();
    if (Application::getInstance().getAppCommunicator().getRank() == 0) {
        logging::debug(prompt + ". General information about the stream: ");
        logging::debug("Stream filename: " + stream.getFilename());
        logging::debug("Frame number: " + std::to_string(stream.getFrameNumber()));
        if (stream.getMode() == data::stream::Stream::Read){
            logging::debug("Reading mode");
        }
        if (stream.getMode() == data::stream::Stream::Write){
            logging::debug("Writing mode");
        }
        logging::debug("Sample rate: " + std::to_string(stream.getSampleRate()));
        if (stream.isAutoopen()){
            logging::debug("Autoopen enabled");
        } else {
            logging::debug("Autoopen disabled");
        }
        if (stream.isOpened()){
            logging::debug("Stream status: \033[32mOPENED\033[0m");
        } else {
            logging::debug("Stream statusL \033[31mCLOSED\033[0m");
        }
        logging::debug("");
    }
    logging::exit();
}


void test_main(){
    using namespace std;

    logging::progress(0, 1, "Generation and save of the primary stream");

    const double height = 1.0, width = 1.0, length = 5.0;
    const double sample_rate = 10.0;
    const int final_timestamp = (int)(length * sample_rate);
    const double moving_rate = 2.0;
    const double spatial_frequency = 2.0;

    mpi::Communicator& comm = Application::getInstance().getAppCommunicator();
    data::ContiguousMatrix stream_source(comm, 20, 20, width, height);
    data::stream::BinStream stream(&stream_source, "sample-stream.bin",
            data::stream::Stream::Write, sample_rate);
    data::reader::PngReader reader("test.png");
    reader.setColormap(data::reader::PngReader::JetColormap); // this reader was added just for check

    print_stream_info(stream, "Stream was just opened");

    for (int ts = 0; ts < final_timestamp; ++ts){
        for (auto a = stream_source.begin(); a != stream_source.end(); ++a){
            double y = a.getRowUm();
            double time = (double)ts / sample_rate;
            *a = cos(2 * M_PI * (moving_rate * time - spatial_frequency * y));
        }
        stream.write();
        if (ts == 0){
            reader.save(stream_source);
        }
    }

    print_stream_info(stream, "All data has been written to the stream");

    logging::progress(1, 1);

}