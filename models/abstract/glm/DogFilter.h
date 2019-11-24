//
// Created by serik1987 on 24.11.2019.
//

#ifndef MPI2_DOGFILTER_H
#define MPI2_DOGFILTER_H

#include "SpatialKernel.h"
#include "../../../processors/Equation.h"

namespace equ {

    class DogFilter: public Equation {
    private:
        SpatialKernel *excitation = nullptr, *inhibition = nullptr;

        double darkRate = -1.0, excitatoryWeight = -1.0, inhibitoryWeight = -3.0, threshold = 1000.0;

    protected:
        [[nodiscard]] std::string getProcessorName() override { return "equ::DogFilter"; }
        bool isOutputContiguous() override { return false; };
        void loadParameterList(const param::Object& source) override;
        void broadcastParameterList() override;
        void setParameter(const std::string& name, const void* pvalue) override;
        void finalizeProcessor(bool destruct = false) noexcept override;

    public:
        explicit DogFilter(mpi::Communicator& comm): Equation(comm), Processor(comm) {};

        class incorrect_spatial_kernel: public simulation_exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "Input processors to the DOG filter are absent or incorrect";
            }
        };

        class negative_dark_rate: public simulation_exception {
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "Dark firing rate of the GLM is negative";
            }
        };

        class negative_threshold: public simulation_exception {
        public:
            [[nodiscard]] const char* what() const noexcept override {
                return "Theshold for the GLM DOG filter is negative";
            }
        };

        class input_dimensions_mismatch: public simulation_exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "Dimensions and/or sizes are not the same for the excitation and the inhibition of the DOG";
            }
        };

        ~DogFilter() override{
            finalizeProcessor(true);
        }

        /**
         * Initializes the DOG processor
         */
        void initialize() override;

        void update(double time) override;

        /**
         * Sets the input spatial kernels
         *
         * @param excitatoryKernel spatial kernel that describes the excitation processes
         * @param inhibitoryKernel spatial kernel that describes the inhibitory processes
         */
        void setSpatialKernels(SpatialKernel* excitatoryKernel, SpatialKernel* inhibitoryKernel);

        /**
         *
         * @return pointer to the excitatory spatial kernel
         */
        SpatialKernel* getExcitatoryKernel() {
            if (excitation == nullptr){
                throw incorrect_spatial_kernel();
            }
            return excitation;
        }

        /**
         *
         * @return pointer to the inhibitory spatial kernel
         */
        SpatialKernel* getInhibitoryKernel() {
            if (inhibition == nullptr){
                throw incorrect_spatial_kernel();
            }
            return inhibition; }

        /**
         *
         * @return background firing rate (when all processors are zero)
         */
        [[nodiscard]] double getDarkRate() const { return darkRate; }

        /**
         * Returns the weight of the excitatory center. The parameter is recommended to be 1.0 for
         * the ON layers and -1.0 for the OFF layers
         *
         * @return the excitatory weight (dimensionless)
         */
        [[nodiscard]] double getExcitatoryWeight() const { return excitatoryWeight; }

        /**
         * Returns the weight of the inhibitory center. The parameter is recommended to be positive for
         * the ON layers and negative for the OFF layers
         *
         * @return the inhibitory weight (dimensionless)
         */
        [[nodiscard]] double getInhibitoryWeight() const { return inhibitoryWeight; }

        [[nodiscard]] double getThreshold() const { return threshold; }

        /**
         * Sets new value of the dark firing rate
         *
         * @param value dark firing rate in Hz
         */
        void setDarkRate(double value){
            if (value >= 0){
                darkRate = value;
            } else {
                throw negative_dark_rate();
            }
        }

        /**
         * Sets new value of the excitatory kernel weight
         *
         * @param value excitatory kernel weight (recommended: 1.0 for ON cells, -1.0 for OFF cells)
         */
        void setExcitatoryWeight(double value) { excitatoryWeight = value; }

        /**
         * Sets new value of the inhibitory kernel weight
         *
         * @param value inhibitory kernel weight (recommended: positive for ON cells, negative for OFF cells)
         */
        void setInhibitoryWeight(double value) { inhibitoryWeight = value; }

        /**
         * Sets the threshold. All firing rates below the threshold will be suppressed
         *
         * @param value threshold in Hz
         */
        void setThreshold(double value){
            if (value >= 0){
                threshold = value;
            } else {
                throw negative_threshold();
            }
        }
    };

}


#endif //MPI2_DOGFILTER_H
