//
// Created by serik1987 on 12.11.2019.
//

#include "StationaryGaborGrating.h"

namespace stim{

    void StationaryGaborGrating::fillStimulusMatrix() {
        double ctheta = cos(getOrientation());
        double stheta = sin(getOrientation());
        double sf = getSpatialFrequency();
        double phi0 = getSpatialPhase();
        double L = getLuminance();
        double C = getContrast();

        for (auto pix = stimulusMatrix->begin(); pix != stimulusMatrix->end(); ++pix){
            double x = pix.getColumnUm();
            double y = pix.getRowUm();
            // double l = x * ctheta + y * stheta;
            double w = x * stheta - y * ctheta;
            *pix = C * cos(2 * M_PI * sf * w + phi0) + L;
            if (*pix<0) *pix = 0;
            if (*pix>1) *pix = 1;
        }
    }

}