//
// Created by serik1987 on 12.11.2019.
//

#ifndef MPI2_STATIONARYDOTSTIMULUS_H
#define MPI2_STATIONARYDOTSTIMULUS_H

#include "StationaryStimulus.h"

namespace stim {

    class dot_abscissa_error: public simulation_exception{
    public:
        [[nodiscard]] const char* what() const noexcept override{
            return "Abscissa of the stationary dot is out of the visual field";
        }
    };

    class dot_ordinate_error: public simulation_exception{
    public:
        [[nodiscard]] const char* what() const noexcept override{
            return "Ordinate of the stationary dot is out of the visual field";
        }
    };

    class dot_radius_error: public simulation_exception{
    public:
        [[nodiscard]] const char* what() const noexcept override{
            return "Radius of the visual field is too large (> 1/2 visual field) or negative";
        }
    };

    class StationaryDotStimulus : public StationaryStimulus {
    private:
        double x = 0.0;
        double y = 0.0;
        double radius = 0.0;

    protected:
        std::string getProcessorName() override { return "stim::StationaryDot"; }
        void loadStationaryStimulusParameters(const param::Object& source) override;
        void broadcastStationaryStimulusParameters() override;
        void setStationaryStimulusParameter(const std::string& name, const void* pvalue) override;
        void fillStimulusMatrix() override;

    public:
        explicit StationaryDotStimulus(mpi::Communicator& comm): StationaryStimulus(comm) {};

        /**
         *
         * @return abscissa of the dot center
         */
        [[nodiscard]] double getX() { return x; }

        /**
         *
         * @return ordinate of the dot center
         */
        [[nodiscard]] double getY() { return y; }

        /**
         *
         * @return radius of the dot
         */
        [[nodiscard]] double getRadius() { return radius; }

        /**
         * Sets abscissa of the dot center
         *
         * @param value abscissa of the dot center in degrees
         */
        void setX(double value){
            if (std::abs(value) < 0.5 * getSizeX()){
                x = value;
            } else {
                throw dot_abscissa_error();
            }
        }

        /**
         * Sets ordinate of the dot center
         *
         * @param value ordinate of the dot center in degrees
         */
        void setY(double value){
            if (std::abs(value) < 0.5 * getSizeY()){
                y = value;
            } else {
                throw dot_ordinate_error();
            }
        }

        /**
         * Sets radius of the dot
         *
         * @param value dot radius in degrees
         */
        void setRadius(double value){
            double fieldSize = 0.0;
            if (getSizeX() < getSizeY()){
                fieldSize = getSizeX();
            } else {
                fieldSize = getSizeY();
            }

            if (value > 0 && value <= fieldSize){
                radius = value;
            } else {
                throw dot_radius_error();
            }
        }

    };

}


#endif //MPI2_STATIONARYDOTSTIMULUS_H
