//
// Created by serik1987 on 14.11.2019.
//

#ifndef MPI2_STREAMSTIMULUS_H
#define MPI2_STREAMSTIMULUS_H

#include "MovingStimulus.h"
#include "../data/stream/BinStream.h"

namespace stim {

    class StreamStimulus: public MovingStimulus {
    private:
        std::string filename;
        data::stream::BinStream* stream = nullptr;
        bool streamFinished = false;

    protected:
        [[nodiscard]] std::string getProcessorName() override { return "stim::StreamStimulus"; }
        void loadMovingStimulusParameters(const param::Object& source) override;
        void broadcastMovingStimulusParameters() override;
        void setMovingStimulusParameter(const std::string& name, const void* pvalue) override;
        void updateMovingStimulus(double relativeTime) override;

        void initializeExtraBuffer() override;
        void finalizeExtraBuffer(bool destruct = false) override;

    public:
        class stream_opening_failed: public simulation_exception{
        private:
            std::string message;

        public:
            explicit stream_opening_failed(const std::exception& e){
                message = std::string("Stream stimulus is failed to open the stream: ") + e.what();
            }

            [[nodiscard]] const char* what() const noexcept override{
                return message.c_str();
            }
        };

        class stream_size_mismatch: public simulation_exception{
        private:
            std::string message;

        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "Stream is opened successfully, but its size is different from the matrix size";
            }
        };

        explicit StreamStimulus(mpi::Communicator& comm): MovingStimulus(comm) {};

        ~StreamStimulus() override {
            finalizeExtraBuffer(true);
        }

        /**
         *
         * @return stream filename to open. Tne file will seek in the current folder
         */
        [[nodiscard]] const std::string& getFilename() const {
            return filename;
        }

        /**
         * Sets the stream filename
         *
         * @param filename name of a valid file. The file will seek in the current folder
         */
        void setFilename(const std::string& fname) { filename = fname; }
    };

}


#endif //MPI2_STREAMSTIMULUS_H
