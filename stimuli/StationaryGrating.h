//
// Created by serik1987 on 11.11.2019.
//

#ifndef MPI2_STATIONARYGRATING_H
#define MPI2_STATIONARYGRATING_H

#include "StationaryStimulus.h"

namespace stim {

    class negative_spatial_frequency: public simulation_exception{
    public:
        const char* what() const noexcept override{
            return "Negative or zero value of stimulus spatial frequency";
        }
    };

    /**
     * Represents a single stationary grating
     */
    class StationaryGrating: public StationaryStimulus {
    private:
        double orientation = 0.0;
        double spatialPhase = 0.0;
        double spatialFrequency = 0.0;

    protected:
        void loadStationaryStimulusParameters(const param::Object& source) override;
        void broadcastStationaryStimulusParameters() override;
        void setStationaryStimulusParameter(const std::string& name, const void* pvalue) override;

    public:
        explicit StationaryGrating(mpi::Communicator& comm): StationaryStimulus(comm) {};

        /**
         *
         * @return grating orientation in radians
         */
        double getOrientation(){ return orientation; }

        /**
         *
         * @return spatial phase in radians
         */
        double getSpatialPhase() { return spatialPhase; }

        /**
         *
         * @return spatial frequency in cpd
         */
        double getSpatialFrequency() { return spatialFrequency; }

        /**
         * Sets the grating orientation
         *
         * @param value grating orientation in degrees
         */
        void setOrientation(double value) { orientation = value; }

        /**
         * Sets the spatial phase in rad
         *
         * @param value spatial phase in rad
         */
        void setSpatialPhase(double value) { spatialPhase = value; }

        void setSpatialFrequency(double value){
            if (value > 0){
                spatialFrequency = value;
            } else {
                throw negative_spatial_frequency();
            }
        }
    };

}


#endif //MPI2_STATIONARYGRATING_H
