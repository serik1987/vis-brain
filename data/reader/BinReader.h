//
// Created by serik1987 on 12.10.2019.
//

#ifndef MPI2_BINREADER_H
#define MPI2_BINREADER_H

#include "../Matrix.h"
#include "Saver.h"
#include "Loader.h"

namespace data::reader {

    /**
     * This sort of reader is responsible to read and write the data in vis-brain's native file format
     * The reader is readable and writeable
     */
    class BinReader: public Saver, public Loader {
    protected:
        void write(data::Matrix& matrix) override;
        data::LocalMatrix* read(mpi::Communicator& comm) override;
    public:
        BinReader(const std::string& filename): Saver(filename), Loader(filename), Reader(filename) {};
        virtual ~BinReader() = default;
    };

}


#endif //MPI2_BINREADER_H
