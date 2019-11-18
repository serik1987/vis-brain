//
// Created by serik1987 on 18.11.2019.
//

#include "GlmLayer.h"
#include "StimulusSaturation.h"
#include "../../../log/output.h"

namespace net{

    equ::Processor *GlmLayer::createGlmMechanism(mpi::Communicator &comm, const std::string &mechanism) {
        using std::string;
        equ::Processor* instance = nullptr;

        size_t delimiter = mechanism.find('.');
        if (delimiter == string::npos){
            throw param::UnknownMechanism("glm:" + mechanism);
        }
        string major_name = mechanism.substr(0, delimiter);
        string minor_name = mechanism.substr(delimiter+1);

        if (major_name == "stimulus_saturation"){
            instance = equ::StimulusSaturation::createStimulusSaturation(comm, minor_name);
        } else {
            throw param::UnknownMechanism("glm:" + mechanism);
        }

        return instance;
    }
}