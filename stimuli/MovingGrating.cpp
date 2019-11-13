//
// Created by serik1987 on 13.11.2019.
//

#include "MovingGrating.h"
#include "../log/output.h"

namespace stim{

    void MovingGrating::loadMovingStimulusParameters(const param::Object &source) {
        using std::to_string;

        loadGratingStimulusParameters(source);
        setTemporalFrequency(source.getFloatField("temporal_frequency"));
        logging::info("Temporal frequency, Hz: " + to_string(getTemporalFrequency()));
    }

    void MovingGrating::broadcastMovingStimulusParameters() {
        Application& app = Application::getInstance();

        broadcastGratingStimulusParameters();
        app.broadcastDouble(temporalFrequency, 0);
    }

    void MovingGrating::setMovingStimulusParameter(const std::string &name, const void *pvalue) {
        if (name == "spatial_frequency" || name == "spatial_phase" || name == "orientation") {
            setGratingStimulusParameter(name, pvalue);
        } else if (name == "temporal_frequency") {
            setTemporalFrequency(*(double*)&pvalue);
        } else {
            throw param::IncorrectParameterName(name, "moving grating");
        }
    }
}