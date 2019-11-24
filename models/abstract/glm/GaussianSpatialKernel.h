//
// Created by serik1987 on 23.11.2019.
//

#ifndef MPI2_GAUSSIANSPATIALKERNEL_H
#define MPI2_GAUSSIANSPATIALKERNEL_H

#include "SpatialKernel.h"

namespace equ {

    class GaussianSpatialKernel: public SpatialKernel {
    private:
        double radius;

    protected:
        [[nodiscard]] std::string getProcessorName() override { return "equ::GaussianSpatialKernel"; }
        void loadParameterList(const param::Object& source) override;
        void broadcastParameterList() override;
        void setParameter(const std::string& name, const void* pvalue) override;
        void initializeSpatialKernel() override;

    public:
        explicit GaussianSpatialKernel(mpi::Communicator& comm): SpatialKernel(comm), Processor(comm) {};

        class negative_radius_error: public simulation_exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "Radius for the spatial gaussian kernel is negative or zero";
            }
        };

        /**
         *
         * @return Kernel half-width in deg
         */
        [[nodiscard]] double getRadius() const {
            return radius;
        }

        void setRadius(double value){
            if (value > 0){
                radius = value;
            } else {
                throw negative_radius_error();
            }
        }
    };

}


#endif //MPI2_GAUSSIANSPATIALKERNEL_H
