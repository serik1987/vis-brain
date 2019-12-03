//
// Created by serik1987 on 01.12.2019.
//

#ifndef MPI2_PRIMARYVSDANALYZER_H
#define MPI2_PRIMARYVSDANALYZER_H

#include "PrimaryAnalyzer.h"
#include "VsdAnalyzer.h"
#include "../data/ContiguousMatrix.h"

namespace analysis {

    /**
     * Applies "virtual" VSD record for recording the layer activity with
     * high temporal and spatial accuracy
     */
    class PrimaryVsdAnalyzer: public PrimaryAnalyzer, public VsdAnalyzer {
    private:
        double imaging_area_size_x;
        double imaging_area_size_y;
        double acquisition_step;
        unsigned long long acquisition_ts;

        data::ContiguousMatrix* buffer = nullptr;

    protected:
        [[nodiscard]] std::string getProcessorName() override { return "analysis::PrimaryVsdAnalyzer"; };
        void loadAnalysisParameters(const param::Object& source) override;
        void broadcastAnalysisParameters() override;
        void setAnalyzerParameters(const std::string& name, const void* pvalue) override;
        bool isOutputContiguous() override { return false; }
        void initializeVsd() override;

        void finalizeProcessor(bool destruct = false) noexcept final;

    public:
        explicit PrimaryVsdAnalyzer(mpi::Communicator& comm): PrimaryAnalyzer(comm), VsdAnalyzer(comm),
            Analyzer(comm) {};

        ~PrimaryVsdAnalyzer() override {
            finalizeProcessor(true);
        }

        /**
         * The method runs each timestamp (after the integration method will be applied
         * and is used to keep the analyser data up to date
         *
         * @param time current time in ms
         */
        void update(double time) override;

        /**
         *
         * @return horizontal size of the imaging area in um or any other units
         */
        [[nodiscard]] double getImagingAreaSizeX() const { return imaging_area_size_x; }

        /**
         *
         * @return vertical size of the imaging area in um or any other units
         */
        [[nodiscard]] double getImagingAreaSizeY() const { return imaging_area_size_y; }

        /**
         *
         * @return acquisition step in ms
         */
        [[nodiscard]] double getAcquisitionStep() const { return acquisition_step; }

        class negative_imaging_area_size_x: public simulation_exception{
        public:
            [[nodiscard]] const char* what() const noexcept override {
                return "Primary VSD analyzer has wrong horizontal VSD area size";
            }
        };

        /**
         * Sets the horizontal size of the imaging area
         *
         * @param value horizontal size of the imaging area in um or degr
         */
        void setImagingAreaSizeX(double value) {
            if (value > 0.0) {
                imaging_area_size_x = value;
            } else {
                throw negative_imaging_area_size_x();
            }
        }

        class negative_imaging_area_size_y: public simulation_exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "Primary VSD analyzer has wrong vertical VSD area size";
            }
        };

        void setImagingAreaSizeY(double value){
            if (value > 0.0){
                imaging_area_size_y = value;
            } else {
                throw negative_imaging_area_size_y();
            }
        }

        class negative_acquisition_step: public simulation_exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "Primary VSD analyzer has negative or zero acquisition step";
            }
        };

        void setAcquisitionStep(double value){
            if (value > 0.0){
                acquisition_step = value;
            } else {
                throw negative_acquisition_step();
            }
        }

        /**
         *
         * @return matrix width in pixels
         */
        [[nodiscard]] int getMatrixWidth() override;

        /**
         *
         * @return matrix height in pixels
         */
        [[nodiscard]] int getMatrixHeight() override;

        /**
         *
         * @return matrix width in um or any other units
         */
        [[nodiscard]] double getMatrixWidthUm() override;

        /**
         *
         * @return matrix height in um or any other units
         */
        [[nodiscard]] double getMatrixHeightUm() override;

        /**
        *
        * @param time iteration time at a current timestamp
        * @return true if the analyzer is ready for update at this iteration, false
        * if the update shall be skipped
        */
        virtual bool isReady(double time) override;
    };

}


#endif //MPI2_PRIMARYVSDANALYZER_H
