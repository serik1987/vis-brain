//
// Created by serik1987 on 13.11.2019.
//

#include <sstream>
#include "StepStimulusParameters.h"
#include "../log/output.h"

namespace stim {


    void StepStimulusParameters::loadStepStimulusParameters(const param::Object &source) {
        using std::endl;

        setPrestimulusEpoch(source.getFloatField("prestimulus_epoch"));
        setStimulusDuration(source.getFloatField("stimulus_duration"));
        setPoststimulusEpoch(source.getFloatField("poststimulus_epoch"));

        std::stringstream ss;
        ss << "Prestimulus epoch, ms: " << getPrestimulusEpoch() << endl;
        ss << "Stimulus duration, ms: " << getStimulusDuration() << endl;
        ss << "Poststimulus epoch, ms: " << getPoststimulusEpoch() << endl;
        ss << "Stimulus start, ms: " << getStimulusStart() << endl;
        ss << "Stimulus finish, ms: " << getStimulusFinish() << endl;
        ss << "Total record length, ms: " << getRecordLength();
        logging::info(ss.str());
    }

    void StepStimulusParameters::broadcastStepStimulusParameter() {
        Application& app = Application::getInstance();
        app.broadcastDouble(prestimulusEpoch, 0);
        app.broadcastDouble(stimulusDuration, 0);
        app.broadcastDouble(poststimulusEpoch, 0);
    }

    void StepStimulusParameters::setStepStimulusParameter(const std::string &name, const void *pvalue) {
        if (name == "prestimulus_epoch"){
            setPrestimulusEpoch(*static_cast<const double*>(pvalue));
        } else if (name == "stimulus_duration") {
            setStimulusDuration(*static_cast<const double *>(pvalue));
        } else if (name == "poststimulus_epoch") {
            setPoststimulusEpoch(*static_cast<const double*>(pvalue));
        }
    }
}