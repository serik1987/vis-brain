//
// Created by serik1987 on 16.11.2019.
//

#include "BoundedStimulus.h"
#include "../exceptions.h"

namespace stim{

    void BoundedStimulus::loadComplexStimulusParameters(const param::Object &source) {

    }

    void BoundedStimulus::broadcastComplexStimulusParameters() {

    }

    void BoundedStimulus::setComplexStimulusParameter(const std::string &name, const void *pvalue) {

    }

    void BoundedStimulus::initializeComplexStimulus() {
        recordLength = 0.0;
        bool is_first = true;

        for (auto pstimulus = inputProcessorBegin(); pstimulus != inputProcessorEnd(); ++pstimulus){
            auto* stimulus = dynamic_cast<Stimulus*>(*pstimulus);
            if (stimulus == nullptr){
                throw WrongStimulus();
            }
            if (is_first){
                recordLength = stimulus->getRecordLength();
                is_first = false;
            } else {
                if (recordLength > stimulus->getRecordLength()){
                    recordLength = stimulus->getRecordLength();
                }
            }
        }
    }

    void BoundedStimulus::update(double time) {
        output->fill(1.0);

        for (auto pstimulus = inputProcessorBegin(); pstimulus != inputProcessorEnd(); ++pstimulus){
            auto* stimulus = dynamic_cast<Stimulus*>(*pstimulus);
            stimulus->update(time);
            double L0 = stimulus->getLuminance();
            auto pix = output->begin();
            auto pix0 = stimulus->getOutput().begin();
            for (; pix != output->end(); ++pix, ++pix0){
                *pix *= *pix0 - L0;
            }
        }

        for (auto pix = output->begin(); pix != output->end(); ++pix){
            *pix += getLuminance();
            if (*pix < 0.0) *pix = 0.0;
            if (*pix > 1.0) *pix = 1.0;
        }
    }

    void BoundedStimulus::finalizeComplexStimulus(bool destruct) {

    }
}