//
// Created by serik1987 on 12.11.2019.
//

#ifndef MPI2_STATIONARYBARSTIMULUS_H
#define MPI2_STATIONARYBARSTIMULUS_H

#include "StationaryStimulus.h"

namespace stim {

    class StationaryBarStimulus: public StationaryStimulus {
    private:
        double length = 0.0;
        double width = 0.0;
        double x = 0.0;
        double y = 0.0;
        double orientation = 0.0;

        double maxSize = 0.0;
        double minSize = 0.0;

    protected:
        [[nodiscard]] std::string getProcessorName() override { return "stim::StationaryBarStimulus"; }
        void loadStationaryStimulusParameters(const param::Object& source) override;
        void broadcastStationaryStimulusParameters() override;
        void setStationaryStimulusParameter(const std::string& name, const void* pvalue) override;
        void fillStimulusMatrix() override;

    public:
        explicit StationaryBarStimulus(mpi::Communicator& comm): StationaryStimulus(comm) {};

        class bar_length_error: public simulation_exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "Length of the stationary bar is too large or non-positive";
            }
        };

        class bar_width_error: public simulation_exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "Width of the stationary bar is too large or non-positive";
            }
        };

        class bar_abscissa_error: public simulation_exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "Abscissa of the bar center is out of range";
            }
        };

        class bar_ordinate_error: public simulation_exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "Ordinate of the bar center is out of range";
            }
        };

        /**
         *
         * @return bar length in degrees
         */
        [[nodiscard]] double getLength() { return length; }

        /**
         *
         * @return bar width in dengrees
         */
        [[nodiscard]] double getWidth() { return width; }

        /**
         *
         * @return abscissa of the bar center
         */
        [[nodiscard]] double getX() { return x; }

        /**
         *
         * @return ordinate of the bar center
         */
        [[nodiscard]] double getY() { return y; }

        /**
         *
         * @return bar orientation in radians
         */
        [[nodiscard]] double getOrientation() { return orientation; }

        /**
         * Sets the bar length in degrees
         *
         * @param value bar length in degrees
         */
        void setLength(double value){
            if (value > 0 && value <= maxSize){
                length = value;
            } else {
                throw bar_length_error();
            }
        }

        /**
         * Sets the bar width
         *
         * @param value bar width in degrees
         */
        void setWidth(double value){
            if (value > 0 && value <= getLength()){
                width = value;
            } else {
                throw bar_width_error();
            }
        }

        /**
         * Sets the abscissa of the bar center
         *
         * @param value abscissa of the bar center in degrees
         */
        void setX(double value){
            if (std::abs(value) <= 0.5 * minSize){
                x = value;
            } else {
                throw bar_abscissa_error();
            }
        }

        /**
         * Sets the ordinate of the bar center
         *
         * @param value ordinate of the bar center in degrees
         */
        void setY(double value){
            if (std::abs(value) <= 0.5 * minSize){
                y = value;
            } else {
                throw bar_ordinate_error();
            }
        }

        /**
         * Stimulus orientation in radians
         */
        void setOrientation(double value){
            orientation = value;
        }
    };

}


#endif //MPI2_STATIONARYBARSTIMULUS_H
