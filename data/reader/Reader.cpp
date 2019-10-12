//
// Created by serik1987 on 12.10.2019.
//

#include "Reader.h"
#include "BinReader.h"

namespace data::reader{
    Reader* Reader::createReader(const std::string &readerType, const std::string &filename) {
        Reader* reader;

        if (readerType == "bin"){
            reader = new BinReader(filename);
        } else {
            throw std::runtime_error("Reader not found");
        }

        return reader;
    }
}
