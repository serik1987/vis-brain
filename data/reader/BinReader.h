//
// Created by serik1987 on 12.10.2019.
//

#ifndef MPI2_BINREADER_H
#define MPI2_BINREADER_H

#include "../Matrix.h"
#include "Saver.h"

namespace data::reader {

    /**
     * This sort of reader is responsible to read and write the data in vis-brain's native file format
     * The reader is readable and writeable
     */
    class BinReader: public Saver {
    protected:
        void write(data::Matrix& matrix) override;
    public:
        BinReader(const std::string& filename): Saver(filename), Reader(filename) {};
        virtual ~BinReader() {}
    };

}


#endif //MPI2_BINREADER_H
