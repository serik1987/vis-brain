//
// Created by serik1987 on 14.11.2019.
//

#include "ExternalStimulus.h"
#include "../Application.h"
#include "../log/output.h"

namespace stim{

    void ExternalStimulus::loadMovingStimulusParameters(const param::Object &source) {
        using std::string;
        std::stringstream ss;
        for (int i=0; i < getParameterNumber(); ++i){
            const char* parameter_name = getParameterName(i);
            setExternalParameter(parameter_name, source.getFloatField(parameter_name));
            ss << parameter_name << ": " << getParameter(parameter_name) << "\n";
        }
        logging::info(ss.str());
    }

    void ExternalStimulus::broadcastMovingStimulusParameters() {
        Application& app = Application::getInstance();
        for (int i=0; i < getParameterNumber(); i++){
            const char* parameter_name = getParameterName(i);
            double parameter_value = getParameter(parameter_name);
            app.broadcastDouble(parameter_value, 0);
            setExternalParameter(parameter_name, parameter_value);
        }
    }

    void ExternalStimulus::setMovingStimulusParameter(const std::string &name, const void *pvalue) {
        setExternalParameter(name.c_str(), *(double*)pvalue);
    }

    void ExternalStimulus::initializeExtraBuffer() {
        initializeExternalMechanism();
    }

    void ExternalStimulus::updateMovingStimulus(double t) {
        double Lmin = getLuminance() - 0.5 * getContrast();
        double C = getContrast();

        for (auto pix = output->begin(); pix != output->end(); ++pix){
            double x = pix.getColumnUm();
            double y = pix.getRowUm();
            double val;
            if (get_stimulus_value(x, y, t, &val)){
                throw get_stimulus_value_error();
            }
            if (val < 0.0) val = 0.0;
            if (val > 1.0) val = 1.0;
            *pix = Lmin + C * val;
        }
    }

    void ExternalStimulus::finalizeExtraBuffer(bool destruct) {
        finalizeExternalMechanism();
    }
}