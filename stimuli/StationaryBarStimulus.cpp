//
// Created by serik1987 on 12.11.2019.
//

#include "StationaryBarStimulus.h"
#include "../Application.h"
#include "../log/output.h"

namespace stim {

    void StationaryBarStimulus::loadStationaryStimulusParameters(const param::Object &source) {
        loadBarStimulusParameters(source, getSizeX(), getSizeY());
    }

    void StationaryBarStimulus::broadcastStationaryStimulusParameters() {
        broadcastBarStimulusParameters();
    }

    void StationaryBarStimulus::setStationaryStimulusParameter(const std::string &name, const void *pvalue) {
        bool is_bar_stimulus_parameter = setBarStimulusParameter(name, pvalue);

         if (!is_bar_stimulus_parameter) {
            throw param::IncorrectParameterName(name, "stationary bar");
        }
    }

    void StationaryBarStimulus::fillStimulusMatrix() {
        double x0 = getX();
        double y0 = getY();
        double l_max = 0.5 * getLength();
        double w_max = 0.5 * getWidth();
        double theta = getOrientation();
        double ctheta = cos(theta);
        double stheta = sin(theta);
        double L = getLuminance();
        double Lmax = L + getContrast();

        for (auto pix = stimulusMatrix->begin(); pix != stimulusMatrix->end(); ++pix){
            double x = pix.getColumnUm();
            double y = pix.getRowUm();
            double l = (x-x0) * ctheta + (y-y0) * stheta;
            double w = (x-x0) * stheta - (y-y0) * ctheta;
            if (std::abs(l) <= l_max && std::abs(w) <= w_max){
                *pix = Lmax;
            } else {
                *pix = L;
            }
            if (*pix < 0.0) *pix = 0.0;
            if (*pix > 1.0) *pix = 1.0;
        }
    }

}