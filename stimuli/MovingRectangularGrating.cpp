//
// Created by serik1987 on 13.11.2019.
//

#include "MovingRectangularGrating.h"


namespace stim {

    void MovingRectangularGrating::updateMovingStimulus(double t) {
        double theta = getOrientation();
        double stheta = sin(theta);
        double ctheta = cos(theta);
        double s = getSpatialFrequency();
        double phase0 = getSpatialPhase() / (2 * M_PI) + 0.001 * getTemporalFrequency() * t - 0.25;
        double Lmax = getLuminance() + 0.5 * getContrast();
        double Lmin = getLuminance() - 0.5 * getContrast();

        for (auto pix = output->begin(); pix != output->end(); ++pix){
            double x = pix.getColumnUm();
            double y = pix.getRowUm();
            double w = x * stheta  - y * ctheta;
            double phase = s * w + phase0;
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