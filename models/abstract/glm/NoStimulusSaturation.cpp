//
// Created by serik1987 on 18.11.2019.
//

#include "NoStimulusSaturation.h"
#include "../../../log/output.h"

namespace equ{

    void NoStimulusSaturation::loadSaturationParameterList(const param::Object &source) {
        logging::info("Saturation type: no stimulus saturation");
    }

    void NoStimulusSaturation::setSaturationParameter(const std::string &name, const void *pvalue) {
        throw param::IncorrectParameterName(name, "default [no] stimulus saturation");
    }
}