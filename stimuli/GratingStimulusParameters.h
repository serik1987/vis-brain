//
// Created by serik1987 on 13.11.2019.
//

#ifndef MPI2_GRATINGSTIMULUSPARAMETERS_H
#define MPI2_GRATINGSTIMULUSPARAMETERS_H

#include "../log/exceptions.h"
#include "../param/Object.h"

namespace stim {

    /**
     * A base class for StationaryGrating and MovingGrating
     */
    class GratingStimulusParameters {
    private:
        double orientation = 0.0;
        double spatialPhase = 0.0;
        double spatialFrequency = 0.0;

    protected:
        void loadGratingStimulusParameters(const param::Object& source);
        void broadcastGratingStimulusParameters();

        /**
         * Sets 'orientation' or 'spatial_frequency', or 'spatial_phase'
         *
         * @param name parameter name
         * @param pvalue pointer to the parameter value
         */
        void setGratingStimulusParameter(const std::string& name, const void* pvalue);

    public:
        class negative_spatial_frequency: public simulation_exception{
        public:
            const char* what() const noexcept override{
                return "Negative or zero value of stimulus spatial frequency";
            }
        };

        /**
        *
        * @return grating orientation in radians
        */
        [[nodiscard]] double getOrientation() const { return orientation; }

        /**
         *
         * @return spatial phase in radians
         */
        [[nodiscard]] double getSpatialPhase() const { return spatialPhase; }

        /**
         *
         * @return spatial frequency in cpd
         */
        [[nodiscard]] double getSpatialFrequency() const { return spatialFrequency; }

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


#endif //MPI2_GRATINGSTIMULUSPARAMETERS_H
