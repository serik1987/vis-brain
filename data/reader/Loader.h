//
// Created by serik1987 on 12.10.2019.
//

#ifndef MPI2_LOADER_H
#define MPI2_LOADER_H

#include "Reader.h"
#include "../LocalMatrix.h"

namespace data::reader {

    /**
     * A base class for all readers that allow data loading
     */
    class Loader: virtual public Reader {
    protected:
        mpi::File* file;

        /**
         * Implementation of a certain reading algorithm assuming that the file has already been opened for reading
         * and represented int the file protected variable, file pointer is at the beginning of the file
         *
         * @param comm communicator that shall be used during creating new matrix
         *
         * @return the matrix object. This is responsibility of this method to create instance of this object and
         * allocate memory for this
         */
        virtual data::LocalMatrix* read(mpi::Communicator& comm) = 0;
    public:
        /**
         *
         * @param filename default filename
         */
        Loader(const std::string& filename): Reader(filename) {}

        /**
         * Loads the matrix from the file
         * This is a collective routine
         *
         * @param comm communicator that shall be assigned to the matrix. The same communicator will also be used
         * for loading by means of collective loading routines
         * @param filename name of the file to load or empty string if default filename (given during the constructor)
         * shall be given
         * @return pointer to the loaded matrix. New data::LocalMatrix instance will be created during the method
         * execution, dynamic memory will be allocated. This is your responsibility to free this dynamic memory
         */
        data::LocalMatrix* load(mpi::Communicator& comm, const std::string& filename = "");
    };

}


#endif //MPI2_LOADER_H
