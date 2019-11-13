//
// Created by serik1987 on 13.11.2019.
//

#include "MovingBarStimulus.h"
#include "../Application.h"
#include "../log/output.h"

namespace stim{

    void MovingBarStimulus::loadMovingStimulusParameters(const param::Object &source) {
        using std::to_string;

        loadBarStimulusParameters(source, getSizeX(), getSizeY());
        setSpeed(source.getFloatField("speed"));
        logging::info("Stimulus speed, dps: " + to_string(getSpeed()));
    }

    void MovingBarStimulus::broadcastMovingStimulusParameters() {
        Application& app = Application::getInstance();
        broadcastBarStimulusParameters();
        app.broadcastDouble(speed, 0);
    }

    void MovingBarStimulus::setMovingStimulusParameter(const std::string &name, const void *pvalue) {
        bool is_bar_param = setBarStimulusParameter(name, pvalue);

        if (!is_bar_param){
            if (name == "speed") {
                setSpeed(*(double*)pvalue);
            } else {
                throw param::IncorrectParameterName(name, "moving bar stimulus");
            }
        }
    }

    void MovingBarStimulus::updateMovingStimulus(double t) {
        double x00 = getX();
        double y00 = getY();
        double v = getSpeed();
        double theta = getOrientation();
        double ctheta = cos(theta);
        double stheta = sin(theta);
        double x0 = x00 - v * stheta * t * 0.001;
        double y0 = y00 + v * ctheta * t * 0.001;
        double Lmin = getLuminance();
        double Lmax = Lmin + getContrast();
        if (Lmax > 1.0) Lmax = 1.0;
        double l_max = 0.5 * getLength();
        double w_max = 0.5 * getWidth();

        for (auto pix = output->begin(); pix != output->end(); ++pix){
            double x = pix.getColumnUm();
            double y = pix.getRowUm();
            double l = (x-x0) * ctheta + (y-y0) * stheta;
            double w = (x-x0) * stheta - (y-y0) * ctheta;
            if (std::abs(l) < l_max && std::abs(w) < w_max){
                *pix = Lmax;
            } else {
                *pix = Lmin;
            }
        }
    }
}