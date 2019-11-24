//
// Created by serik1987 on 18.11.2019.
//

#include "GlmLayer.h"
#include "StimulusSaturation.h"
#include "TemporalKernel.h"
#include "SpatialKernel.h"
#include "DogFilter.h"
#include "../../../log/output.h"

namespace net{

    equ::Processor *GlmLayer::createGlmMechanism(mpi::Communicator &comm, const std::string &mechanism,
            equ::Ode::SolutionParameters parameters) {
        using std::string;
        equ::Processor* instance = nullptr;

        size_t delimiter = mechanism.find('.');
        string major_name;
        string minor_name;
        if (delimiter == string::npos){
            major_name = mechanism;
            minor_name = "";
        } else {
            major_name = mechanism.substr(0, delimiter);
            minor_name = mechanism.substr(delimiter + 1);
        }

        if (major_name == "stimulus_saturation") {
            instance = equ::StimulusSaturation::createStimulusSaturation(comm, minor_name);
        } else if (major_name == "temporal_kernel") {
            instance = equ::TemporalKernel::createTemporalKernel(comm, minor_name, parameters);
        } else if (major_name == "spatial_kernel") {
            instance = equ::SpatialKernel::createSpatialKernel(comm, minor_name);
        } else if (major_name == "dog"){
            instance = new equ::DogFilter(comm);
        } else {
            throw param::UnknownMechanism("glm:" + mechanism);
        }

        return instance;
    }
}