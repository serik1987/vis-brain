//
// Created by serik1987 on 12.10.2019.
//

#include "Reader.h"
#include "BinReader.h"
#include "PngReader.h"

namespace data::reader{
    Reader* Reader::createReader(const std::string &readerType, const std::string &filename) {
        Reader* reader;

        if (readerType == "bin") {
            reader = new BinReader(filename);
        } else if (readerType == "png"){
            reader = new PngReader(filename);
        } else {
            throw std::runtime_error("Reader not found");
        }

        return reader;
    }
}
