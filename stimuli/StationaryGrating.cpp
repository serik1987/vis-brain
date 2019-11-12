//
// Created by serik1987 on 11.11.2019.
//

#include "StationaryGrating.h"
#include "../Application.h"
#include "../log/output.h"

namespace stim{

    void StationaryGrating::loadStationaryStimulusParameters(const param::Object &source) {
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

    void StationaryGrating::broadcastStationaryStimulusParameters() {
        Application& app = Application::getInstance();
        app.broadcastDouble(orientation, 0);
        app.broadcastDouble(spatialPhase, 0);
        app.broadcastDouble(spatialFrequency, 0);
    }

    void StationaryGrating::setStationaryStimulusParameter(const std::string &name, const void *pvalue) {
        if (name == "orientation") {
            setOrientation(*static_cast<const double *>(pvalue));
        } else if (name == "spatial_phase") {
            setSpatialPhase(*static_cast<const double *>(pvalue));
        } else if (name == "spatial_frequency"){
            setSpatialFrequency(*static_cast<const double*>(pvalue));
        } else {
            throw param::IncorrectParameterName(name, "stationary grating");
        }
    }

    void StationaryGrating::fillStimulusMatrix() {
        double ctheta = cos(getOrientation());
        double stheta = sin(getOrientation());
        double sf = getSpatialFrequency();
        double phi0 = getSpatialPhase();
        double L = getLuminance();
        double C = getContrast();

        for (auto pix = stimulusMatrix->begin(); pix != stimulusMatrix->end(); ++pix){
            double x = pix.getRowUm();
            double y = pix.getColumnUm();
            // double l = x * ctheta + y * stheta;
            double w = x * stheta - y * ctheta;
            *pix = C * cos(2 * M_PI * sf * w + phi0) + L;
        }
    }

}