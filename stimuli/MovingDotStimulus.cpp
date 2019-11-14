//
// Created by serik1987 on 14.11.2019.
//

#include "MovingDotStimulus.h"
#include "../log/output.h"

namespace stim{

    void MovingDotStimulus::loadMovingStimulusParameters(const param::Object &source) {
        using std::to_string;

        loadDotStimulusParameters(source, getSizeX(), getSizeY());

        setVelocityX(source.getFloatField("velocity_x"));
        logging::info("Velocity projection on abscissa, dps: " + to_string(getVelocityX()));

        setVelocityY(source.getFloatField("velocity_y"));
        logging::info("Velocity projection on ordinate, dps: " + to_string(getVelocityY()));
    }

    void MovingDotStimulus::broadcastMovingStimulusParameters() {
        Application& app = Application::getInstance();

        broadcastDotStimulusParameters();
        app.broadcastDouble(velocityX, 0);
        app.broadcastDouble(velocityY, 0);
    }

    void MovingDotStimulus::setMovingStimulusParameter(const std::string &name, const void *pvalue) {
        bool isDotParameter = setDotStimulusParameters(name, pvalue);

        if (!isDotParameter){
            if (name == "velocity_x") {
                setVelocityX(*(double *) pvalue);
            } else if (name == "velocity_y") {
                setVelocityY(*(double*)pvalue);
            } else {
                throw param::IncorrectParameterName(name, "moving dot");
            }
        }
    }

    void MovingDotStimulus::updateMovingStimulus(double t) {
        double x0 = getX() + getVelocityX() * t * 0.001;
        double y0 = getY() + getVelocityY() * t * 0.001;
        double R2 = getRadius() * getRadius();
        double Lmin = getLuminance();
        double Lmax = Lmin + getContrast();
        if (Lmax > 1.0) Lmax = 1.0;

        for (auto pix = output->begin(); pix != output->end(); ++pix){
            double x = pix.getColumnUm();
            double y = pix.getRowUm();
            double r2 = (x-x0) * (x-x0) + (y-y0) * (y-y0);

            if (r2 <= R2){
                *pix = Lmax;
            } else {
                *pix = Lmin;
            }
        }
    }
}
