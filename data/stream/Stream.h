//
// Created by serik1987 on 15.10.2019.
//

#ifndef MPI2_STREAM_H
#define MPI2_STREAM_H

#include "../Matrix.h"
#include "../exceptions.h"

namespace data::stream {

    /**
     * Base class for all streams. A stream is a temporary sequence of matrices written into the hard disk.
     * Instances of these classes allows to write the matrix at each timestamp and by this way make so called
     * "result movie" where matrix values play over different times
     *
     * All routines in this base class are collective routines. They shall be called by all processes simultaneously
     */
    class Stream {
    public:
        enum StreamMode {Read, Write};

    private:
        mpi::Communicator& comm;
        data::Matrix* matrix;
        StreamMode mode;
        double sampleRate;
        bool opened;

    protected:
        std::string filename;
        int frameNumber;
        int totalFrames = 0;
        bool autoopen;

        /**
         * Returns the communicator for matrices. For shared write/read functions only
         *
         * @return the communicator itself
         */
        mpi::Communicator& getCommunicator() { return comm; }

        /**
         *
         * @return reference to the matrix
         */
        const data::Matrix& getMatrix() { return *matrix; }

        /* The following methods are to be implemented when you want to create a custom stream */

        /**
         * Opens an existent stream for reading
         * The method shall also read header and footer from this file
         * Please, make sure that totalFrames protected variable is initialized during the file opening. If this
         * is not so, reading a stream frame by means of read() routine will always generate an exception
         *
         * If exception is thrown during the opening the method shall provide all closing operations automatically
         */
        virtual void openStreamFile() = 0;

        /**
         * Creates new stream files
         * The method shall also write the header.
         *
         * If an exception is thown the method shall close all file handles automatically
         */
        virtual void createStreamFile() = 0;

        /**
         * Writes the matrix to the stream
         *
         * @param matrix matrix given. The method shall write just this matrix
         */
        virtual void writeMatrix(data::Matrix* matrix) = 0;

        /**
         * Reads the matrix from the stream and puts the data to a variable matrix
         *
         * @param matrix
         */
        virtual void readMatrix(data::Matrix* matrix) = 0;

        /**
         * Close stream file that is opened for reading
         */
         virtual void finishReading() = 0;

         /**
          * Closes stream file shat is opened for writing
          */
         virtual void finishWriting() = 0;

    public:

        /**
         * Initializes the stream and put all parameters to it
         * This is a collective routine. It shall be called by all processes simultaneously
         *
         * @param matrix pointer to the matrix of the streamer. The streamer allows to write the matrix you gave
         * as well as any other matrices which sizes are the same to the size of this matrix
         * @param filename filename where the matrices shall be written.
         * @param mode Read if you want to read the data from existent stream. The program will generate an exception
         * if the stream file doesn't exist
         * Write if you want to write the data to new file. The program will throw an exception if file exists
         * @param frameRate sample rate, to be written to a file
         * @param autoopen true if you want to open the stream file during the construction of the object,
         * false if the stream shall be opened later. In this case you shall open the stream manually by call of open()
         * method
         */
        Stream(data::Matrix* matrix, const std::string& filename, StreamMode mode, double frameRate, bool autoopen = true);

        Stream(const Stream& other) = delete;

        virtual ~Stream() {}

        /**
         * Opens the stream file for reading or writing.
         * The method will be called automatically when the stream instance is created with autoopen parameter set to true
         *
         * If the stream has already been closed, don't try to open the stream for writing again. This will cause
         * an exception
         */
        void open() {
            if (opened) return; // Not necessary to open an opened stream again
            if (mode == Read) {
                openStreamFile();
                opened = true;
                frameNumber = 0;
            }
            if (mode == Write) {
                try {
                    createStreamFile();
                    opened = true;
                } catch (std::exception& e) {
                    logging::enter();
                    logging::warning(std::string("Write to the stream was switched off due to the following error: ") +
                        e.what() + ". The simulation results may not be saved.");
                    logging::exit();
                    opened = false;
                }
            }
            frameNumber = 0;
        };

        /**
         * Closes the stream that is previously opened.
         * If the stream is created with autoopen flag, the method will be called automatically during the object
         * destruction.
         *
         * If the stream has already been closed the method will do nothing.
         */
        void close(){
            if (!opened) return;
            try{
                if (mode == Read) finishReading();
                if (mode == Write) finishWriting();
            } catch (std::exception& e){
                logging::enter();
                logging::warning(
                        std::string("The stream was corrupted due to the following error occured during the stream close: ")
                    + e.what() + ". This means that simulation data may be fully lost while all files where they put may "+
                    "be useless for further analysis");
                logging::exit();
            }
            opened = false;
        }

        /**
         * Write a single matrix to the hard disk.
         * When the streamer is in reading mode the method throws an exception
         * Writing will be switched off if error occured during the stream opening or writing. In this case
         * any following write(...) method will do nothing
         *
         * @param output_data pointer to the matrix to be written or nullptr if you want to write the matrix pointed
         * during the construction
         */
        void write(data::Matrix* output_data = nullptr){
            if (mode == Read){
                throw stream_not_opened();
            }
            if (!opened) return;
            if (output_data == nullptr){
                output_data = matrix;
            }
            try{
                if (output_data->getHeight() != matrix->getHeight()
                    || output_data->getWidth() != matrix->getWidth()){
                    throw matrix_dimensions_mismatch();
                }
                writeMatrix(output_data);
            } catch (std::exception& e){
                logging::enter();
                logging::warning(
                        std::string("Writing simulation data to the output stream was failed due to the following error: ") +
                        e.what() + ". The output stream was closed. Simulation data may partly of fully lost");
                logging::exit();
                close();
            }
            ++frameNumber;
        }

        /**
         * Reads a single matrix from the hard disk
         * If stream is opened in writing mode the method
         *
         * @param input_data pointer to the matrix where all read data shall be placed
         */
        void read(data::Matrix* input_data = nullptr){
            if (mode == Write || !opened){
                throw stream_not_opened();
            }
            if (frameNumber >= totalFrames){
                throw end_of_stream_reached();
            }
            if (input_data == nullptr){
                input_data = matrix;
            }
            if (input_data->getHeight() != matrix->getHeight() ||
                input_data->getWidth() != matrix->getWidth()){
                throw matrix_dimensions_mismatch();
            }
            readMatrix(input_data);
            ++frameNumber;
        }

        /**
         * Returns the filename. All stream data will be save to this file. This property is set during construction
         * of the object
         *
         * @return string containing the filename
         */
        const std::string& getFilename(){ return filename; }

        /**
         * Returns total number of matrices written or read
         *
         * @return total number of matrices
         */
        int getFrameNumber() { return frameNumber; }

        /**
         * Stream mode. Stream mode is set during the object construction and can't be changed
         *
         * @return either Stream::Read or Stream::Write
         */
        StreamMode getMode() { return mode; }

        /**
         * Returns the sample rate. The parameter is initialized during the construction of the stream and
         * can't be changed during the stream job. This will be written to a file.
         *
         * @return sample rate
         */
        double getSampleRate() { return sampleRate; }

        /**
         * Returns true if autoopen is on
         *
         * @return true or false
         */
        bool isAutoopen(){ return autoopen; }

        /**
         * Returns whether the stream is opened or closed
         *
         * @return true or false
         */
        bool isOpened() { return opened;}

    };

}


#endif //MPI2_STREAM_H
