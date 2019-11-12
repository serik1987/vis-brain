//
// Created by serik1987 on 12.11.2019.
//

#include "StationaryRectangularGrating.h"

namespace stim{

    void StationaryRectangularGrating::fillStimulusMatrix() {
        double theta = getOrientation();
        double ctheta = cos(theta);
        double stheta = sin(theta);
        double sf = getSpatialFrequency();
        double phi0 = getSpatialPhase()/2/M_PI;
        double C = getContrast();
        double L = getLuminance();
        double Lmax = L + C;
        double Lmin = L - C;

        for (auto pix = stimulusMatrix->begin(); pix != stimulusMatrix->end(); ++pix){
            double x = pix.getColumnUm();
            double y = pix.getRowUm();
            double w = x * stheta - y * ctheta;
            double phase = sf * w + phi0 + 0.25;
            phase -= floor(phase);
            if (phase <= 0.5){
                *pix = Lmax;
            } else {
                *pix = Lmin;
            }
            if (*pix < 0.0) *pix = 0.0;
            if (*pix > 1.0) *pix = 1.0;
        }
    }

}