//
// Created by serik1987 on 11.11.2019.
//

#include "StationaryGrating.h"
#include "../Application.h"

namespace stim{

    void StationaryGrating::loadStationaryStimulusParameters(const param::Object &source) {
        loadGratingStimulusParameters(source);
    }

    void StationaryGrating::broadcastStationaryStimulusParameters() {
        broadcastGratingStimulusParameters();
    }

    void StationaryGrating::setStationaryStimulusParameter(const std::string &name, const void *pvalue) {
        if (name=="orientation" || name=="spatial_frequency" || name=="spatial_phase"){
            setGratingStimulusParameter(name, pvalue);
        } else {
            throw param::IncorrectParameterName(name, "stationary grating");
        }
    }

}