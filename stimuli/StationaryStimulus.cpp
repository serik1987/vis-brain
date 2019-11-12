//
// Created by serik1987 on 11.11.2019.
//

#include "StationaryStimulus.h"
#include "../Application.h"
#include "../log/output.h"
#include "StationaryGrating.h"

namespace stim{

    void StationaryStimulus::loadStimulusParameters(const param::Object &source) {
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

        loadStationaryStimulusParameters(source);
    }

    void StationaryStimulus::broadcastStimulusParameters() {
        Application& app = Application::getInstance();
        app.broadcastDouble(prestimulusEpoch, 0);
        app.broadcastDouble(stimulusDuration, 0);
        app.broadcastDouble(poststimulusEpoch, 0);
        broadcastStationaryStimulusParameters();
    }

    void StationaryStimulus::setStimulusParameter(const std::string &name, const void *pvalue) {
        if (name == "prestimulus_epoch"){
            setPrestimulusEpoch(*static_cast<const double*>(pvalue));
        } else if (name == "stimulus_duration") {
            setStimulusDuration(*static_cast<const double *>(pvalue));
        } else if (name == "poststimulus_epoch") {
            setPoststimulusEpoch(*static_cast<const double*>(pvalue));
        } else {
            setStationaryStimulusParameter(name, pvalue);
        }
    }

    void StationaryStimulus::initializeStimulus() {
        stimulusMatrix = new data::LocalMatrix(getCommunicator(), getGridX(),
                getGridY(), getSizeX(), getSizeY());
        time = 0.0;
        output->fill(getLuminance());
        fillStimulusMatrix();
    }

    void StationaryStimulus::finalizeProcessor(bool destruct) noexcept{
        delete stimulusMatrix;
        stimulusMatrix = nullptr;
    }

    data::Matrix& StationaryStimulus::getOutput(){
        data::Matrix* result;
        if (time >= getStimulusStart() && time < getStimulusFinish()){
            result = stimulusMatrix;
        } else {
            result = output;
        }

        return *result;
    }

    StationaryStimulus* StationaryStimulus::createStationaryStimulus(mpi::Communicator &comm,
                                                                     const std::string &mechanism) {
        StationaryStimulus* stimulus;

        if (mechanism == "grating"){
            logging::info("Stationary grating");
            stimulus = new StationaryGrating(comm);
        } else {
            throw param::UnknownMechanism();
        }

        return stimulus;
    }

}