//
// Created by serik1987 on 11.11.2019.
//

#include "StationaryStimulus.h"
#include "../Application.h"
#include "../log/output.h"
#include "StationaryGaborGrating.h"
#include "StationaryRectangularGrating.h"
#include "StationaryDotStimulus.h"
#include "StationaryBarStimulus.h"
#include "StationaryReaderStimulus.h"

namespace stim{

    void StationaryStimulus::loadStimulusParameters(const param::Object &source) {
        loadStepStimulusParameters(source);
        loadStationaryStimulusParameters(source);
    }

    void StationaryStimulus::broadcastStimulusParameters() {
        broadcastStepStimulusParameter();
        broadcastStationaryStimulusParameters();
    }

    void StationaryStimulus::setStimulusParameter(const std::string &name, const void *pvalue) {
        if (name == "prestimulus_epoch" || name == "stimulus_duration" || name == "poststimulus_epoch"){
            setStepStimulusParameter(name, pvalue);
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

        if (output == nullptr || stimulusMatrix == nullptr){
            throw uninitialized_processor();
        }
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

        if (mechanism == "gabor-grating") {
            logging::info("Stationary gabor grating");
            stimulus = new StationaryGaborGrating(comm);
        } else if (mechanism == "rectangular-grating") {
            logging::info("Stationary rectangular grating");
            stimulus = new StationaryRectangularGrating(comm);
        } else if (mechanism == "dot") {
            logging::info("Stationary dot");
            stimulus = new StationaryDotStimulus(comm);
        } else if (mechanism == "bar") {
            logging::info("Stationary bar");
            stimulus = new StationaryBarStimulus(comm);
        } else if (mechanism == "reader") {
            logging::info("External stimulus from the vis-brain .bin file");
            stimulus = new StationaryReaderStimulus(comm);
        } else {
            throw param::UnknownMechanism("stimulus:stationary."+mechanism);
        }

        return stimulus;
    }

}