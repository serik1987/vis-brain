//
// Created by serik1987 on 14.11.2019.
//

#include "DotStimulusParameters.h"
#include "../log/output.h"

namespace stim {

    void DotStimulusParameters::loadDotStimulusParameters(const param::Object &source, double sizeX, double sizeY) {
        this->sizeX = sizeX;
        this->sizeY = sizeY;

        using std::to_string;

        setX(source.getFloatField("x"));
        logging::info("x = " + to_string(getX()));

        setY(source.getFloatField("y"));
        logging::info("y = " + to_string(getY()));

        setRadius(source.getFloatField("radius"));
        logging::info("Dot radius, degr: " + to_string(getRadius()));
    }

    void DotStimulusParameters::broadcastDotStimulusParameters() {
        Application& app = Application::getInstance();
        app.broadcastDouble(x, 0);
        app.broadcastDouble(y, 0);
        app.broadcastDouble(radius, 0);
    }

    bool DotStimulusParameters::setDotStimulusParameters(const std::string &name, const void *pvalue) {
        bool paramSetResult = false;

        if (name == "x"){
            setX(*(double*)pvalue);
            paramSetResult = true;
        } else if (name == "y") {
            setY(*(double *) pvalue);
            paramSetResult = true;
        } else if (name == "radius"){
            setRadius(*(double*)pvalue);
            paramSetResult = true;
        }

        return paramSetResult;
    }
}