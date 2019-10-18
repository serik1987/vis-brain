//
// Created by serik1987 on 18.10.2019.
//

#ifndef MPI2_EXTERNALSTREAM_H
#define MPI2_EXTERNALSTREAM_H

#include "Stream.h"
#include "../ContiguousMatrix.h"

namespace data::stream {

    /**
     * A base class for set of streamers that use external writing methods that are not supported by the MPI machinery
     * Please, be care that all matrices to write are assumed to be synchronized. All reading matrices are synchronized
     * by the definition.
     */
    class ExternalStream: public Stream {
    private:
        int root = 0;

    protected:

        /**
         * When creating custom external stream, this method shall be re-implemented in such a way as it opens
         * an external stream file for reading and reads file header and footer.
         */
        virtual void openExternalStream() = 0;

        /**
         * When creating custom external stream, this method shall be re-implemented in such a way as it creates
         * new stream file for writing and writes the file header
         */
        virtual void createExternalStream() = 0;

        /**
         * When creating custom external stream, this method shall be re-implemented in such a way that it writes
         * the contiguous matrix to the hard disk drive
         * This method is executed by the root process only.
         *
         * @param contiguousMatrix the matrix to write
         */
        virtual void writeContiguousMatrix(data::ContiguousMatrix* contiguousMatrix) = 0;

        /**
         * When creating custom external stream, this method shall be re-implemeted in such a way that it reads the
         * contiguous matrix from the hard disk drive
         * The routine is executed by the root process only
         *
         * @param contiguousMatrix matrix where all reading values will be placed
         */
        virtual void readContiguousMatrix(data::ContiguousMatrix* contiguousMatrix) = 0;

        /**
         * When creating custon external stream, this method shall be re-implemented in such a way as it
         * closes the file associated with a reading stream and de-allocates all reading buffers.
         */
        virtual void closeExternalReadingStream() = 0;

        /**
         * When creating custom external stream, re-implement this method in such a way as it writes the trailer
         * to the external stream file, closes this file and de-allocates all buffers associated with this stream
         */
        virtual void closeExternalWritingStream() = 0;

        /**
         * Writes some matrix to the output stream.
         * This is a final method, not to be re-implemented. Use writeContiguousMatrix instead.
         *
         * @param matrix matrix to write
         */
        void writeMatrix(data::Matrix* matrix) override final{
            data::ContiguousMatrix* contiguousMatrix;
            contiguousMatrix = dynamic_cast<data::ContiguousMatrix*>(matrix);
            if (contiguousMatrix == nullptr){
                throw external_stream_error();
            }
            if (matrix->getCommunicator().getRank() == root){
                writeContiguousMatrix(contiguousMatrix);
            }
        };

        /**
         * Reads some matrix from the input stream.
         * This is a final method, not to be re-implemented. Use readContiguousMatrix instead
         *
         * @param matrix matrix to read
         */
        void readMatrix(data::Matrix* matrix) override final{
            data::ContiguousMatrix* contiguousMatrix;
            contiguousMatrix = dynamic_cast<data::ContiguousMatrix*>(matrix);
            if (contiguousMatrix == nullptr){
                throw external_stream_error();
            }
            if (matrix->getCommunicator().getRank() == root){
                readContiguousMatrix(contiguousMatrix);
            }
        }

        /**
         * Opens the stream file for reading.
         * This is a final method, not for implementing. Re-implement openExternalStream instead.
         */
        void openStreamFile() override final {
            if (getCommunicator().getRank() == root){
                openExternalStream();
            }
        }

        /**
         * Creates new stream and opens it for writing
         * This is a final method, don't re-implement this. Re-implement createExternalStream instead
         */
        void createStreamFile() override final {
            if (getCommunicator().getRank() == root){
                createExternalStream();
            }
        }

        /**
         * Finishes reading an external stream file
         * This is a final method, don't re=implement this. Re-implement closeExternalReadingStream instead
         */
        void finishReading() override final {
            if (getCommunicator().getRank() == root){
                closeExternalReadingStream();
            }
        }

        /**
         * Closes the external writing stream
         * This is a final method, don't re-implement this. Re-implement closeExternalWritingStream instead
         */
        void finishWriting() override final {
            if (getCommunicator().getRank() == root){
                closeExternalWritingStream();
            }
        }

    public:
        ExternalStream(data::ContiguousMatrix* matrix, const std::string& filename, StreamMode mode, double frameRate,
                bool autoopen = true): Stream(matrix, filename, mode, frameRate, autoopen) {};

        ExternalStream(const ExternalStream& other) = delete;

        /**
         * returns the root process.
         * The root process is a process that performs an actual save to the hard disk drive
         *
         * @return rank of the root process
         */
        int getRoot() { return root; }

        /**
         * Sets the root process.
         * The root process is a process that performs an actual save of the data into hard disk drive.
         *
         * @param root rank of the root process
         */
        void setRoot(int root) { this->root = root; }
    };

}


#endif //MPI2_EXTERNALSTREAM_H
