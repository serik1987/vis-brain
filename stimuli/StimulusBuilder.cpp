//
// Created by serik1987 on 30.11.2019.
//

#include "StimulusBuilder.h"
#include "../Application.h"
#include "../processors/Processor.h"

namespace stim{

    void StimulusBuilder::loadParameterList(const param::Object &source) {
        mechanism = source.getStringField("mechanism");
        auto* proc = equ::Processor::createProcessor(comm, mechanism);
        stimulus = dynamic_cast<Stimulus*>(proc);
        if (stimulus == nullptr){
            throw param::WrongMechanism();
        }
        stimulus->loadParameters(source);
    }

    void StimulusBuilder::broadcastParameterList() {
        auto& app = Application::getInstance();
        app.broadcastString(mechanism, 0);
        if (app.getAppCommunicator().getRank() != 0){
            auto* proc = equ::Processor::createProcessor(comm, mechanism);
            stimulus = dynamic_cast<Stimulus*>(proc);
        }
        stimulus->broadcastParameters();
    }
}