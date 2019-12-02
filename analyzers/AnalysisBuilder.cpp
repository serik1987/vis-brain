//
// Created by serik1987 on 01.12.2019.
//

#include "AnalysisBuilder.h"
#include "../log/output.h"
#include "../processors/Processor.h"

namespace analysis{

    void AnalysisBuilder::loadParameterList(const param::Object &source) {
        mechanism = source.getStringField("mechanism");
        auto* cp = equ::Processor::createProcessor(comm, mechanism);
        analyzer = dynamic_cast<Analyzer*>(cp);
        if (analyzer == nullptr){
            throw incorrect_analyzer();
        }
        analyzer->loadParameters(source);
    }

    void AnalysisBuilder::broadcastParameterList() {
        Application& app = Application::getInstance();
        app.broadcastString(mechanism, 0);
        if (analyzer == nullptr){
            auto* cp = equ::Processor::createProcessor(comm, mechanism);
            analyzer = dynamic_cast<Analyzer*>(cp);
        }
        analyzer->broadcastParameters();
    }
}