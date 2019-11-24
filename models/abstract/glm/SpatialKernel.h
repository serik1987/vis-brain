//
// Created by serik1987 on 23.11.2019.
//

#ifndef MPI2_SPATIALKERNEL_H
#define MPI2_SPATIALKERNEL_H

#include "../../../processors/Equation.h"
#include "../../../data/ContiguousMatrix.h"
#include "TemporalKernel.h"

namespace equ {

    class SpatialKernel: public Equation {
    private:
        data::ContiguousMatrix* buffer = nullptr;

    protected:
        bool isOutputContiguous() override { return false; };
        void finalizeProcessor(bool destruct = false) noexcept override;

        /**
         * Creates the new matrix pointer by the kernel protected pointer (before call of this function
         * the value of kernel is always nullptr, then, fills it by the kernel values.
         * Doesn't synchronizes the kernel.
         * This is not a collective routine.
         */
        virtual void initializeSpatialKernel() = 0;
        data::ContiguousMatrix* kernel = nullptr;

    public:
        explicit SpatialKernel(mpi::Communicator& comm): Equation(comm), Processor(comm) {};

        ~SpatialKernel() override{
            finalizeProcessor(true);
        }

        class incorrect_temporal_kernel: public simulation_exception {
        public:
            const char* what() const noexcept override {
                return "Spatial kernel has not valid input. Its initialization failed";
            }
        };

        /**
         * Creates new spatial kernel
         *
         * @param comm communicator
         * @param mechanism mechanism minor name
         * @return pointer to the SpatialKernel
         * @throws UnknownM<echanism when the mechanism doesn't exist
         */
        static SpatialKernel* createSpatialKernel(mpi::Communicator& comm, const std::string& mechanism);

        /**
         * Sets an appropriate temporal kernel. Temporal kernel is the only input processor to the spatial kernel
         *
         * @param temporalKernel pointer to the temporal kernel
         */
        void setTemporalKernel(TemporalKernel* temporalKernel);

        /**
         *
         * @return pointer to the input temporal kernel or nullptr is the kernel is not assigned
         */
        TemporalKernel* getTemporalKernel() { return dynamic_cast<TemporalKernel*>(*inputProcessorBegin()); }

        /**
         * Initializes the processor
         * This is a collective routine
         */
        void initialize() override;

        /**
         * Updates the processor
         * This is a collective routine
         *
         * @param time absolute time in ms (the parameter is useless because such information is not needed)
         */
        void update(double time) override;

        data::ContiguousMatrix& getBuffer() { return *buffer; }

        data::ContiguousMatrix& getKernel() { return *kernel; }
    };

}


#endif //MPI2_SPATIALKERNEL_H
