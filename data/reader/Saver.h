//
// Created by serik1987 on 12.10.2019.
//

#ifndef MPI2_SAVER_H
#define MPI2_SAVER_H

#include "Reader.h"
#include "../Matrix.h"
#include "../../mpi/File.h"

namespace data::reader {

    /**
     * Base class for all matrix readers that allow to save the matrix into. Please note all file paths are estimated
     * relatively to the output folder. See name of the output folder in the output messages
     */
    class Saver: virtual public Reader {
    protected:
        mpi::File* savingFile;

        /**
         * The Saver is an abstract class, so, you can't you it to save the data. It doesn't implements a saving
         * algorithm. Such an algorithm is implemented in any derived class where this method is overloaded.
         * During the overloading please, use mpi::File* savingFile variable
         *
         * @param matrix matrix to save
         */
        virtual void write(data::Matrix& matrix)=0;

        /**
         * Provides an immediate routines for file saving. The method is called by the save(...) method. save(...)
         * method evaluates relative path to the file to save and call this method to save the data into a particular
         * file. If your Reader access the file through MPI file routines this is not necessary to pay attention on
         * this method. Otherwise, this method shall be overriden. During the overriding the file handle will no longer
         * saves into savingFile protected variable, so you may simply forget about this.
         *
         * @param matrix matrix to save
         * @param fullfile final path to the saving file
         */
        virtual void saveFile(data::Matrix& matrix, const std::string& fullfile);
    public:
        /**
         * Constructor: the same arguments as in Reader
         *
         * @param filename default filename
         */
        Saver(const std::string& filename): Reader(filename) {}
        virtual ~Saver() = default;

        /**
         * Provides a routine for saving file into an appropriate matrix
         * This is a collective routine
         *
         * @param matrix reference to the matrix
         * @param filename the filename or empty string if you want to use default filename (see constructor help
         * for details)
         */
        void save(data::Matrix& matrix, const std::string& filename = "");
    };

}


#endif //MPI2_SAVER_H
