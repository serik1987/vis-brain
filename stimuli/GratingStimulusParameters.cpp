//
// Created by serik1987 on 13.11.2019.
//

#include <sstream>
#include "GratingStimulusParameters.h"
#include "../log/output.h"

namespace stim{

    void GratingStimulusParameters::loadGratingStimulusParameters(const param::Object &source) {
        using std::endl;
        std::stringstream ss;

        setOrientation(source.getFloatField("orientation"));
        ss << "Orientation, rad: " << getOrientation() << endl;
        setSpatialPhase(source.getFloatField("spatial_phase"));
        ss << "Spatial phase at the center, rad: " << getSpatialPhase() << endl;
        setSpatialFrequency(source.getFloatField("spatial_frequency"));
        ss << "Spatial frequency, cpd: " << getSpatialFrequency() << endl;

        logging::info(ss.str());
    }

    void GratingStimulusParameters::broadcastGratingStimulusParameters() {
        Application& app = Application::getInstance();
        app.broadcastDouble(orientation, 0);
        app.broadcastDouble(spatialPhase, 0);
        app.broadcastDouble(spatialFrequency, 0);
    }

    void GratingStimulusParameters::setGratingStimulusParameter(const std::string &name, const void *pvalue) {
        if (name == "orientation") {
            setOrientation(*static_cast<const double *>(pvalue));
        } else if (name == "spatial_phase") {
            setSpatialPhase(*static_cast<const double *>(pvalue));
        } else if (name == "spatial_frequency"){
            setSpatialFrequency(*static_cast<const double*>(pvalue));
        }
    }
}