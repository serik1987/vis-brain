//
// Created by serik1987 on 12.11.2019.
//

#include "StationaryDotStimulus.h"
#include "../log/output.h"


namespace stim {


    void StationaryDotStimulus::loadStationaryStimulusParameters(const param::Object &source) {
        loadDotStimulusParameters(source, getSizeX(), getSizeY());
    }

    void StationaryDotStimulus::broadcastStationaryStimulusParameters() {
        broadcastDotStimulusParameters();
    }

    void StationaryDotStimulus::setStationaryStimulusParameter(const std::string &name, const void *pvalue) {
        bool paramSetResult = setDotStimulusParameters(name, pvalue);
        if (!paramSetResult){
            {
                throw param::IncorrectParameterName(name, "stationary dot");
            }
        }
    }

    void StationaryDotStimulus::fillStimulusMatrix() {
        double x0 = getX();
        double y0 = getY();
        double R2 = getRadius() * getRadius();
        double L = getLuminance();
        double Lmax = L + getContrast();

        for (auto pix = stimulusMatrix->begin(); pix != stimulusMatrix->end(); ++pix){
            double x = pix.getColumnUm();
            double y = pix.getRowUm();
            double r2 = (x-x0) * (x-x0) + (y-y0) * (y-y0);
            if (r2 <= R2) {
                *pix = Lmax;
            } else {
                *pix = L;
            }
            if (*pix < 0.0) *pix = 0.0;
            if (*pix > 1.0) *pix = 1.0;
        }
    }
}