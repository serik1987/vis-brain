//
// Created by serik1987 on 23.11.2019.
//

#include "SpatialKernel.h"
#include "GaussianSpatialKernel.h"
#include "../../../log/output.h"
#include "../../../data/LocalMatrix.h"

namespace equ{

    SpatialKernel *SpatialKernel::createSpatialKernel(mpi::Communicator& comm, const std::string &mechanism) {
        SpatialKernel* kernel = nullptr;

        if (mechanism == "gaussian"){
            kernel = new GaussianSpatialKernel(comm);
        } else {
            throw param::UnknownMechanism("glm:spatial_kernel." + mechanism);
        }

        return kernel;
    }

    void SpatialKernel::initialize() {
        TemporalKernel* input_processor = getTemporalKernel();
        if (input_processor == nullptr){
            throw incorrect_temporal_kernel();
        }
        auto& input = input_processor->getOutput();
        output = new data::LocalMatrix(getCommunicator(), input.getWidth(), input.getHeight(),
                input.getWidthUm(), input.getHeightUm());
        buffer = new data::ContiguousMatrix(getCommunicator(), input.getWidth(), input.getHeight(),
                input.getWidthUm(), input.getHeightUm());
        initializeSpatialKernel();
        kernel->synchronize();
    }

    void SpatialKernel::update(double time) {
        auto input = getTemporalKernel()->getOutput().cbegin();
        auto buf = buffer->begin();
        for (; buf != buffer->end(); ++buf, ++input){
            *buf = *input;
        }
        buffer->synchronize();
        output->convolve(*kernel, *buffer, true);
    }

    void SpatialKernel::finalizeProcessor(bool destruct) noexcept {
        delete buffer;
        buffer = nullptr;
        delete kernel;
        kernel = nullptr;
    }

    void SpatialKernel::setTemporalKernel(TemporalKernel *temporalKernel) {
        auto* old_kernel = *inputProcessorBegin();
        if (old_kernel != nullptr){
            removeInputProcessor(old_kernel);
        }
        addInputProcessor(temporalKernel);
    }
}