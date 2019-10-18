//
// Created by serik1987 on 18.10.2019.
//

#ifndef MPI2_AVISTREAM_H
#define MPI2_AVISTREAM_H

#include "ExternalStream.h"
#include "../graph/ColorAxis.h"

namespace data::stream {

    class AviStream: public ExternalStream {
    private:
        data::graph::ColorAxis caxis;

    protected:
        void openExternalStream() override {
            throw write_only_stream_error();
        }

        void readContiguousMatrix(data::ContiguousMatrix* matrix) override {
            throw write_only_stream_error();
        }

        void closeExternalReadingStream() {
            throw write_only_stream_error();
        }

        void createExternalStream() override;
        void writeContiguousMatrix(data::ContiguousMatrix* matrix) override;
        void closeExternalWritingStream() override;

    public:
        /**
         * Creates new AVI stream.
         * Please, note that you don't have to mention the stream mode because reading AVI stream is not implemented.
         * This is because AVI files don't have all data that are necessary to restore the matrix from the stream.
         *
         * @param matrix the default matrix. All other matrices to write to the stream shall have the same shape and
         * the same communicator than this matrix. Certain matrix values are not influence on the stream content
         * @param filename filename associated with the stream. This file will be created during the stream opening
         * @param frameRate number of frames per second
         * @param autoopen if true the stream will be opened during construction of the object and will be closed
         * during the object destruction. If false, this is your responsibility to open/close the stream manually.
         */
        AviStream(data::ContiguousMatrix* matrix, const std::string& filename, double frameRate, bool autoopen = true):
            ExternalStream(matrix, filename, Stream::Write, frameRate, autoopen) {
            if (autoopen){
                open();
            }
        };

        AviStream(const AviStream& other) = delete;

        virtual ~AviStream() {
            if (isAutoopen()){
                close();
            }
        };

        /**
         * Returns the color axis.
         * Color axis is an auxiliary object that you can use for adjusting additional parameters of the stream
         * such as colormap and color limits. See reference for this class for details.
         *
         * @return reference to data::graph::ColorAxis object
         */
        data::graph::ColorAxis& getColorAxis() { return caxis; }
    };

}


#endif //MPI2_AVISTREAM_H
