//
// Created by serik1987 on 13.11.2019.
//

#include "MovingStimulus.h"
#include "MovingGaborGrating.h"
#include "../log/output.h"


namespace stim {

    MovingStimulus *MovingStimulus::createMovingStimulus(mpi::Communicator &comm, const std::string &mechanism) {
        MovingStimulus* stimulus = nullptr;

        if (mechanism == "gabor-grating") {
            stimulus = new MovingGaborGrating(comm);
        } else {
            throw param::UnknownMechanism("stimulus:moving." + mechanism);
        }

        return stimulus;
    }

    void MovingStimulus::initializeStimulus() {
        showStimulus = false;
        meanLuminance = new data::LocalMatrix(getCommunicator(), getGridX(), getGridY(),
                getSizeX(), getSizeY(), getLuminance());
        output = new data::LocalMatrix(getCommunicator(), getGridX(), getGridY(),
                getSizeX(), getSizeY(), 0.0);
    }

    void MovingStimulus::finalizeProcessor(bool destruct) noexcept {
        delete meanLuminance;
        meanLuminance = nullptr;
    }

    void MovingStimulus::update(double t){
        if (t >= getStimulusStart() && t < getStimulusFinish()){
            showStimulus = true;
            updateMovingStimulus(t - getStimulusStart());
        } else {
            showStimulus = false;
        }
    }

    void MovingStimulus::loadStimulusParameters(const param::Object &source) {
        loadStepStimulusParameters(source);
        loadMovingStimulusParameters(source);
    }

    void MovingStimulus::broadcastStimulusParameters() {
        broadcastStepStimulusParameter();
        broadcastMovingStimulusParameters();
    }

    void MovingStimulus::setStimulusParameter(const std::string &name, const void *pvalue) {
        if (name == "prestimulus_epoch" || name == "stimulus_duration" || name == "poststimulus_epoch"){
            setStepStimulusParameter(name, pvalue);
        } else {
            setMovingStimulusParameter(name, pvalue);
        }
    }
}