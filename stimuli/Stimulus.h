//
// Created by serik1987 on 11.11.2019.
//

#ifndef MPI2_STIMULUS_H
#define MPI2_STIMULUS_H

#include "../processors/Equation.h"

namespace stim {

    class negative_stimulus_dimensions: public simulation_exception{
    public:
        const char* what() const noexcept override{
            return "Stimulus size in pixels (grid_x or grid_y) or stimulus size in degrees (size_x or size_y) is zero"
                   " or negative";
        }
    };

    class incorrect_luminance: public simulation_exception{
    public:
        const char* what() const noexcept override{
            return "Stimulus luminance is set incorrectly";
        }
    };

    class incorrect_contrast: public simulation_exception{
    public:
        const char* what() const noexcept override{
            return "Stimulus contrast is set incorrectly";
        }
    };

    /**
     * A base class for all visual stimuli
     */
    class Stimulus: public equ::Equation {
    private:
        int gridX = 0, gridY = 0;
        double sizeX = 0.0, sizeY = 0.0, luminance = 0.0, contrast = 0.0;

    protected:
        void loadParameterList(const param::Object& source) override;
        void broadcastParameterList() override;

        /**
         * Reads all other stimulus parameters except 'type', 'mechanism', 'grid_x', 'grid_y',
         * 'size_x', 'size_y', 'luminance', 'contrast'
         *
         * @param source an object that contains the JS object
         */
        virtual void loadStimulusParameters(const param::Object& source)=0;

        /**
         * Broadcasts all other stimulus parameters except type', 'mechanism', 'grid_x', 'grid_y',
         * 'size_x', 'size_y', 'luminance', 'contrast'
         */
        virtual void broadcastStimulusParameters()=0;

        /**
         * Sets stimulus parameters except type', 'mechanism', 'grid_x', 'grid_y',
         * 'size_x', 'size_y', 'luminance', 'contrast'
         *
         * @param name parameter name
         * @param pvalue pointer to the parameter value
         */
        virtual void setStimulusParameter(const std::string& name, const void* pvalue)=0;

        bool isOutputContiguous() override { return false; }

        /**
         * Performs all initialization routines for the stimulus except creating output matrix
         */
        virtual void initializeStimulus() = 0;

    public:
        explicit Stimulus(mpi::Communicator& comm): Equation(comm) {};

        /**
         * Initializes a stimulus
         * This is a collective routine. It shall be run by all processors containing in the communicator
         * simultaneously
         */
        void initialize() override;

        /**
         * Sets an arbitrary parameter of the stimulus
         *
         * @param name parameter name
         * @param pvalue parameter value
         */
        void setParameter(const std::string& name, void* pvalue) override;

        /**
         *
         * @return record length in ms. The record length is defined by the stimulus length
         */
        virtual double getRecordLength()=0;

        /**
         *
         * @return stimulus width in pixels
         */
        [[nodiscard]] int getGridX() const { return gridX; }

        /**
         * Sets the stimulus width in pixels
         *
         * @param value stimulus width in pixels
         */
        void setGridX(int value) {
            if (value <= 0){
                throw negative_stimulus_dimensions();
            }
            gridX = value;
        }

        /**
         *
         * @return stimulus height in pixels
         */
        [[nodiscard]] int getGridY() const { return gridY; }

        /**
         * Sets the stimulus height in pixels
         *
         * @param value stimulus height in pixels
         */
        void setGridY(int value){
            if (value <= 0){
                throw negative_stimulus_dimensions();
            }
            gridY = value;
        }

        /**
         *
         * @return stimulus width in degrees
         */
        [[nodiscard]] double getSizeX() const { return sizeX; }

        /**
         * Sets the stimulus width in degrees
         *
         * @param value stimulus width in degrees
         */
        void setSizeX(double value){
            if (value <=  0){
                throw negative_stimulus_dimensions();
            }
            sizeX = value;
        }

        /**
         *
         * @return stimulus height in degrees
         */
        [[nodiscard]] double getSizeY() const { return sizeY; }

        /**
         * Sets the stimulus height in degrees
         *
         * @param value stimulus height in degrees
         */
        void setSizeY(double value){
            if (value <= 0){
                throw negative_stimulus_dimensions();
            }
            sizeY = value;
        }

        /**
         *
         * @return mean luminance of the stimulus
         */
        [[nodiscard]] double getLuminance() const { return luminance; }

        /**
         * Sets the relative luminance of the stimulus
         *
         * @param value stimulus luminance
         */
        void setLuminance(double value){
            if (value >= 0.0 && value <= 1.0){
                luminance = value;
            } else {
                throw incorrect_luminance();
            }
        }

        /**
         *
         * @return mean contrast of the stimulus
         */
        [[nodiscard]] double getContrast() const { return contrast; }

        /**
         * Sets the stimulus contrast
         *
         * @param value stimulus contrast
         */
        void setContrast(double value){
            if (value >= 0.0 && value <= 1.0){
                contrast = value;
            } else {
                throw incorrect_contrast();
            }
        }
    };

}


#endif //MPI2_STIMULUS_H
