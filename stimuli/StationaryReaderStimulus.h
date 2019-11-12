//
// Created by serik1987 on 12.11.2019.
//

#ifndef MPI2_STATIONARYREADERSTIMULUS_H
#define MPI2_STATIONARYREADERSTIMULUS_H

#include "StationaryStimulus.h"

namespace stim {

    class StationaryReaderStimulus: public StationaryStimulus {
    private:
        std::string filename = "";

    protected:
        std::string getProcessorName() override { return "stim::StationaryReaderStimulus"; }
        void loadStationaryStimulusParameters(const param::Object& source) override;
        void broadcastStationaryStimulusParameters() override;
        void setStationaryStimulusParameter(const std::string& name, const void* pvalue) override;
        void fillStimulusMatrix() override;

    public:
        explicit StationaryReaderStimulus(mpi::Communicator& comm): StationaryStimulus(comm) {};

        class reading_failed: public simulation_exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "failure to read stationary reader stimulus from the file";
            }
        };

        class reader_dimensions_mismatch: public simulation_exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "The matrix has read with dimensions that are differed from dimensions given by the operator";
            }
        };

        /**
         *
         * @return name of a file to open
         */
        [[nodiscard]] const std::string& getFilename() { return filename; }

        /**
         * Sets the filename from which the stimulus shall be read
         *
         * @param value name of the valid file
         */
        void setFilename(const std::string& value){
            filename = value;
        }
    };

}


#endif //MPI2_STATIONARYREADERSTIMULUS_H
