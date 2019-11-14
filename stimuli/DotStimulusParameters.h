//
// Created by serik1987 on 14.11.2019.
//

#ifndef MPI2_DOTSTIMULUSPARAMETERS_H
#define MPI2_DOTSTIMULUSPARAMETERS_H

#include <cmath>
#include "../log/exceptions.h"
#include "../param/Object.h"

namespace stim {

    /**
     * A base class for StationaryDotStimulus and MovingDotStimulus
     */
    class DotStimulusParameters {
    private:
        double x = 0.0;
        double y = 0.0;
        double radius = 0.0;
        double sizeX = 0.0;
        double sizeY = 0.0;

    protected:
        /**
         * loads x, y and radius
         *
         * @param source source of these parameters
         * @param sizeX visual field width in degrees
         * @param sizeY visual field height in degrees
         */
        void loadDotStimulusParameters(const param::Object& source, double sizeX, double sizeY);

        /**
         * Broadcasts x, y and radius
         */
        void broadcastDotStimulusParameters();

        /**
         * Sets either x, or y or radius parameter
         *
         * @param name parameter name: "x" or "y" or "radius"
         * @param pvalue pointer to the parameter value
         * @return true if the parameter has set successfully, false for failure
         */
        bool setDotStimulusParameters(const std::string& name, const void* pvalue);


    public:
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

        /**
         *
         * @return abscissa of the dot center
         */
        [[nodiscard]] double getX() const { return x; }

        /**
         *
         * @return ordinate of the dot center
         */
        [[nodiscard]] double getY() const { return y; }

        /**
         *
         * @return radius of the dot
         */
        [[nodiscard]] double getRadius() const { return radius; }

        /**
         * Sets abscissa of the dot center
         *
         * @param value abscissa of the dot center in degrees
         */
        void setX(double value){
            if (std::abs(value) < 0.5 * sizeX){
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
            if (std::abs(value) < 0.5 * sizeY){
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
            double fieldSize;
            if (sizeX < sizeY){
                fieldSize = sizeX;
            } else {
                fieldSize = sizeY;
            }

            if (value > 0 && value <= fieldSize){
                radius = value;
            } else {
                throw dot_radius_error();
            }
        }
    };

}


#endif //MPI2_DOTSTIMULUSPARAMETERS_H
