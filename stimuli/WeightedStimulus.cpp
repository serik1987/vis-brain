//
// Created by serik1987 on 16.11.2019.
//

#include "WeightedStimulus.h"
#include "../log/output.h"

namespace stim{

    void WeightedStimulus::loadComplexStimulusParameters(const param::Object &source) {
        param::Object weights_object = source.getObjectField("weights");
        uint32_t weight_number = weights_object.getPropertyNumber();
        if (weight_number != getInputProcessorNumber()){
            throw weight_vector_inconsistency();
        }
        weights.reserve(weight_number);

        for (uint32_t index = 0; index < weight_number; ++index){
            double weight = weights_object.getFloatField(index);
            weights.push_back(weight);
        }
    }

    void WeightedStimulus::broadcastComplexStimulusParameters() {
        Application& app = Application::getInstance();
        int rank = app.getAppCommunicator().getRank();
        int number = getInputProcessorNumber();
        if (rank != 0){
            weights.insert(weights.end(), number, 0.0);
        }
        double* buffer = &weights[0];
        app.getAppCommunicator().broadcast(buffer, number, MPI_DOUBLE, 0);
    }

    void WeightedStimulus::setComplexStimulusParameter(const std::string &name, const void *pvalue) {
        throw param::IncorrectParameterName(name, "weighted stimulus");
    }

    void WeightedStimulus::initializeComplexStimulus() {
        recordLength = 0.0;
        for (auto it = inputProcessorBegin(); it != inputProcessorEnd(); ++it){
            auto* stimulus = dynamic_cast<Stimulus*>(*it);
            if (stimulus == nullptr){
                throw WrongStimulus();
            }
            if (stimulus->getRecordLength() > recordLength){
                recordLength = stimulus->getRecordLength();
            }
        }
    }

    void WeightedStimulus::update(double time) {
        auto pweight = weights.begin();
        auto pstimulus = inputProcessorBegin();
        double Lmin = getLuminance() - 0.5 * getContrast();
        double C = getContrast();
        output->fill(0.0);

        for (; pstimulus != inputProcessorEnd(); ++pweight, ++pstimulus){
            double weight = *pweight;
            auto* stimulus = dynamic_cast<Stimulus*>(*pstimulus);
            if (time < stimulus->getRecordLength()){
                stimulus->update(time);
                auto pix = output->begin();
                auto pix0 = stimulus->getOutput().cbegin();
                for (; pix != output->end(); ++pix, ++pix0){
                    *pix += weight * *pix0;
                }
            }
        }

        for (auto pix = output->begin(); pix != output->end(); ++pix){
            if (*pix < 0.0) *pix = 0.0;
            if (*pix > 1.0) *pix = 1.0;
            *pix = Lmin + C * *pix;
        }
    }

    void WeightedStimulus::finalizeComplexStimulus(bool) {}
};