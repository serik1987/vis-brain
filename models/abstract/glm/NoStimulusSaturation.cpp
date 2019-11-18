//
// Created by serik1987 on 18.11.2019.
//

#include "NoStimulusSaturation.h"

namespace equ{

    void NoStimulusSaturation::setSaturationParameter(const std::string &name, const void *pvalue) {
        throw param::IncorrectParameterName(name, "default [no] stimulus saturation");
    }
}