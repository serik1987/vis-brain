//
// Created by serik1987 on 18.11.2019.
//

#include "BrokenLineStimulusSaturation.h"
#include "../../../log/output.h"

namespace equ{

    void BrokenLineStimulusSaturation::loadSaturationParameterList(const param::Object &source) {
        setMaxCurrent(source.getFloatField("max_current"));
        logging::info("Maximum current, nA: " + std::to_string(getMaxCurrent()));
    }

    void BrokenLineStimulusSaturation::broadcastSaturationParameterList() {
        Application::getInstance().broadcastDouble(maxCurrent, 0);
    }

    void BrokenLineStimulusSaturation::setSaturationParameter(const std::string &name, const void *pvalue) {
        if (name == "max_current"){
            setMaxCurrent(*(double*)pvalue);
        } else {
            throw param::IncorrectParameterName(name, "broken line saturation");
        }
    }

    double BrokenLineStimulusSaturation::getSaturationOutput(double saturationInput) {
        double saturationOutput = saturationInput;

        if (saturationOutput > getMaxCurrent()){
            saturationOutput = getMaxCurrent();
        }

        return saturationOutput;
    }
}