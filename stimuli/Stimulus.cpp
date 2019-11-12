//
// Created by serik1987 on 11.11.2019.
//

#include "Stimulus.h"
#include "../data/LocalMatrix.h"

namespace stim{

    void Stimulus::loadParameterList(const param::Object &source) {
        using std::endl;
        std::stringstream ss;

        setGridX(source.getIntegerField("grid_x"));
        setGridY(source.getIntegerField(("grid_y")));
        ss << "Stimulus dimensions: " << getGridX() << "x" << getGridY() << " pixels\n";
        setSizeX(source.getFloatField("size_x"));
        setSizeY(source.getFloatField("size_y"));
        ss << "Stimulus size: " << getSizeX() << "x" << getSizeY() << " degrees\n";
        setLuminance(source.getFloatField("luminance"));
        ss << "Luminance: " << getLuminance() << endl;
        setContrast(source.getFloatField("contrast"));
        ss << "Contrast: " << getContrast();

        logging::info(ss.str());
        loadStimulusParameters(source);
    }

    void Stimulus::broadcastParameterList() {
        Application& app = Application::getInstance();
        app.broadcastInteger(gridX, 0);
        app.broadcastInteger(gridY, 0);
        app.broadcastDouble(sizeX, 0);
        app.broadcastDouble(sizeY, 0);
        app.broadcastDouble(luminance, 0);
        app.broadcastDouble(contrast, 0);
        broadcastStimulusParameters();
    }

    void Stimulus::setParameter(const std::string &name, void *pvalue) {
        if (name == "grid_x") {
            setGridX(*static_cast<int*>(pvalue));
        } else if (name == "grid_y") {
            setGridY(*static_cast<int *>(pvalue));
        } else if (name == "size_x") {
            setSizeX(*static_cast<double *>(pvalue));
        } else if (name == "size_y") {
            setSizeY(*static_cast<double *>(pvalue));
        } else if (name == "luminance") {
            setLuminance(*static_cast<double *>(pvalue));
        } else if (name == "contrast") {
            setContrast(*static_cast<double*>(pvalue));
        } else {
            setStimulusParameter(name, pvalue);
        }
    }

    void Stimulus::initialize(){
        output = new data::LocalMatrix(getCommunicator(), getGridX(), getGridY(),
                getSizeX(), getSizeY());
        initializeStimulus();
    }

}
