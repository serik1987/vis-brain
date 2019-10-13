//
// Created by serik1987 on 12.10.2019.
//

#ifndef MPI2_EXTERNALSAVER_H
#define MPI2_EXTERNALSAVER_H

#include "Saver.h"

namespace data::reader {

    /**
     * A base class for all readers that provides data save where saving routines were delivered by some
     * external library. This is why these readers are unable to provide parallel data save using MPI routines.
     * During the file save by means of the ExternalSaver all data will be transmitted to so called root process
     * (see getRoot and setRoot methods for details). This process will provide all save routines while all other
     * processes will do nothing
     */
    class ExternalSaver: virtual public Saver {
    protected:
        int root = 0;
        data::ContiguousMatrix* buffer;
        std::string filename;

        void saveFile(data::Matrix& matrix, const std::string& current_filename) override;
    public:
        ExternalSaver(const std::string& filename): Saver(filename), Reader(filename)  {}
        ~ExternalSaver() override = default;

        /**
         * Returns the root process. The root process is a process that immediately saves the data into hard disk
         *
         * @return the root process
         */
        int getRoot() { return root; }

        /**
         * Sets the root process
         *
         * @param new_root The new root
         */
        void setRoot(int new_root) { root = new_root; }
    };

}


#endif //MPI2_EXTERNALSAVER_H
