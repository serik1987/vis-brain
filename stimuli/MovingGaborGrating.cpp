//
// Created by serik1987 on 13.11.2019.
//

#include "MovingGaborGrating.h"

namespace stim{

    void MovingGaborGrating::updateMovingStimulus(double relativeTime) {
        double L0 = getLuminance();
        double A = 0.5 * getContrast();
        double theta = getOrientation();
        double ctheta = cos(theta);
        double stheta = sin(theta);
        double s = getSpatialFrequency();
        double f = getTemporalFrequency();
        double t = relativeTime * 0.001; // relativeTime is in ms, t shall be in s because f is in Hz
        double phi0 = getSpatialPhase();

        for (auto pix = output->begin(); pix != output->end(); ++pix){
            double x = pix.getColumnUm();
            double y = pix.getRowUm();
            double w = x * stheta - y * ctheta;
            *pix = L0 + A * cos(2*M_PI*s*w + 2*M_PI*f*t + phi0);
            if (*pix < 0.0) *pix = 0.0;
            if (*pix > 1.0) *pix = 1.0;
        }
    }
}