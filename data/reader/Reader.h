//
// Created by serik1987 on 12.10.2019.
//

#ifndef MPI2_READER_H
#define MPI2_READER_H

/**
 * A base class for all matrix readers and writers
 * Matrix reader is an object that allows to write a matrix to a file of a particular format or read the matrix from
 * this format
 */

#include <string>

namespace data::reader {

    class Reader {
    private:
        std::string filename = "";
    public:
        /**
         * Construction
         *
         * @param filename__ default filename to apply
         */
        Reader(const std::string& filename__): filename(filename__) {};
        virtual ~Reader() {};

        /**
         * Returns the current filename
         *
         * @return reference to the string containing the current filename
         */
        const std::string& getFilename() const {return filename; }

        /**
         * Sets the current filename
         *
         * @param new_filename reference to the string containing new filename
         */
        void setFilename(const std::string& new_filename) { filename = new_filename; }

        /**
         * Creates a certain reader by its description
         *
         * @param readerType
         * "bin" - BinReader
         * any other strring corresponds to the ExternalReader
         * @param filename the default filename for the reader
         * @return pointer to this reader. The memory will be allocated for the reader. This is your responsibility
         * to free this memory
         */
        static Reader* createReader(const std::string& readerType, const std::string& filename);
    };

}


#endif //MPI2_READER_H
