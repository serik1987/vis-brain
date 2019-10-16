//
// Created by serik1987 on 15.10.2019.
//

#ifndef MPI2_BINSTREAM_H
#define MPI2_BINSTREAM_H

#include "Stream.h"

namespace data::stream {

    /**
     * Represents a stream that deals with native vis-brain's in format. These data can be loaded into Python
     * by means of vis_brain.streamers.BinStream class.
     */
    class BinStream: public Stream {
    private:
        mpi::File* handle;

    protected:
        void openStreamFile() override;
        void createStreamFile() override;
        void writeMatrix(data::Matrix* matrix) override;
        void readMatrix(data::Matrix* matrix) override;
        void finishReading() override;
        void finishWriting() override;

    public:

        /**
         * Creates a stream. See help for Stream for details
         *
         * @param matrix
         * @param filename
         * @param mode
         * @param sampleRate
         * @param autoopen
         */
        BinStream(data::Matrix* matrix, const std::string& filename, StreamMode mode,
                double sampleRate, bool autoopen = true): Stream(matrix, filename, mode, sampleRate, autoopen) {
            if (autoopen){
                open();
            }
        };

        ~BinStream() {
            if (autoopen){
                close();
            }
        };
    };

}


#endif //MPI2_BINSTREAM_H
