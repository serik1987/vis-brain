//
// Created by serik1987 on 12.11.2019.
//

#include "StationaryBarStimulus.h"
#include "../Application.h"
#include "../log/output.h"

namespace stim {

    void StationaryBarStimulus::loadStationaryStimulusParameters(const param::Object &source) {
        using std::to_string;

        if (getSizeX() > getSizeY()){
            maxSize = getSizeX();
            minSize = getSizeY();
        } else {
            maxSize = getSizeY();
            minSize = getSizeX();
        }

        setLength(source.getFloatField("length"));
        logging::info("Bar length, deg: " + to_string(getLength()));

        setWidth(source.getFloatField("width"));
        logging::info("Bar width, deg: " + to_string(getWidth()));

        setX(source.getFloatField("x"));
        logging::info("X = " + to_string(getX()));

        setY(source.getFloatField("y"));
        logging::info("Y = " + to_string(getY()));

        setOrientation(source.getFloatField("orientation"));
        logging::info("orientation, rad: " + to_string(orientation));
    }

    void StationaryBarStimulus::broadcastStationaryStimulusParameters() {
        Application& app = Application::getInstance();

        app.broadcastDouble(length, 0);
        app.broadcastDouble(width, 0);
        app.broadcastDouble(x, 0);
        app.broadcastDouble(y, 0);
        app.broadcastDouble(orientation, 0);
    }

    void StationaryBarStimulus::setStationaryStimulusParameter(const std::string &name, const void *pvalue) {
        if (name == "length") {
            setLength(*(double *) pvalue);
        } else if (name == "width") {
            setWidth(*(double *) pvalue);
        } else if (name == "x") {
            setX(*(double *) pvalue);
        } else if (name == "y") {
            setY(*(double *) pvalue);
        } else if (name == "orientation") {
            setOrientation(*(double*)pvalue);
        } else {
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