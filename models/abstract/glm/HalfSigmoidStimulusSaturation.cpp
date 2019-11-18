//
// Created by serik1987 on 18.11.2019.
//

#include <cmath>

#include "HalfSigmoidStimulusSaturation.h"
#include "../../../log/output.h"

namespace equ{

    void HalfSigmoidStimulusSaturation::loadSaturationParameterList(const param::Object &source) {
        setMaxCurrent(source.getFloatField("max_current"));
        logging::info("Max current, nA: " + std::to_string(getMaxCurrent()));
    }

    void HalfSigmoidStimulusSaturation::broadcastSaturationParameterList() {
        Application::getInstance().broadcastDouble(maxCurrent, 0);
    }

    void HalfSigmoidStimulusSaturation::setSaturationParameter(const std::string &name, const void *pvalue) {
        if (name == "max_current") {
            setMaxCurrent(*(double*)pvalue);
        } else {
            throw param::IncorrectParameterName(name, "half sigmoid stimulus saturation");
        }
    }

    double HalfSigmoidStimulusSaturation::getSaturationOutput(double saturationInput) {
        double saturationOutput = 0.0;
        double max = getMaxCurrent();

        saturationOutput = max * (2.0 / (1+exp(-2*saturationInput/max)) - 1);

        return saturationOutput;
    }
}