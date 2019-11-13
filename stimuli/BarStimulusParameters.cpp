//
// Created by serik1987 on 13.11.2019.
//

#include "BarStimulusParameters.h"
#include "../log/output.h"

namespace stim{

    void BarStimulusParameters::loadBarStimulusParameters(const param::Object &source, double sizeX, double sizeY) {
        using std::to_string;

        if (sizeX > sizeY){
            maxSize = sizeX;
            minSize = sizeY;
        } else {
            maxSize = sizeY;
            minSize = sizeX;
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

    void BarStimulusParameters::broadcastBarStimulusParameters() {
        Application& app = Application::getInstance();

        app.broadcastDouble(length, 0);
        app.broadcastDouble(width, 0);
        app.broadcastDouble(x, 0);
        app.broadcastDouble(y, 0);
        app.broadcastDouble(orientation, 0);
    }

    bool BarStimulusParameters::setBarStimulusParameter(const std::string &name, const void *pvalue) {
        bool result = false;

        if (name == "length") {
            setLength(*(double *) pvalue);
            result = true;
        } else if (name == "width") {
            setWidth(*(double *) pvalue);
            result = true;
        } else if (name == "x") {
            setX(*(double *) pvalue);
            result = true;
        } else if (name == "y") {
            setY(*(double *) pvalue);
            result = true;
        } else if (name == "orientation") {
            setOrientation(*(double*)pvalue);
            result = true;
        }

        return result;
    }
}