//
// Created by serik1987 on 18.11.2019.
//

#include "StimulusSaturation.h"
#include "NoStimulusSaturation.h"
#include "BrokenLineStimulusSaturation.h"
#include "HalfSigmoidStimulusSaturation.h"
#include "../../../data/LocalMatrix.h"
#include "../../../log/output.h"

namespace equ{

    StimulusSaturation *
    StimulusSaturation::createStimulusSaturation(mpi::Communicator &comm, const std::string &mechanism) {
        StimulusSaturation* saturation = nullptr;

        if (mechanism == "no") {
            saturation = new NoStimulusSaturation(comm);
        } else if (mechanism == "broken_line") {
            saturation = new BrokenLineStimulusSaturation(comm);
        } else if (mechanism == "half_sigmoid") {
            saturation = new HalfSigmoidStimulusSaturation(comm);
        } else {
            throw param::UnknownMechanism("glm:stimulus_saturation." + mechanism);
        }

        return saturation;
    }

    void StimulusSaturation::loadParameterList(const param::Object &source) {
        logging::info("Stimulus saturation parameters:");
        setDarkCurrent(source.getFloatField("dark_current"));
        logging::info("Dark current, nA: " + std::to_string(getDarkCurrent()));
        setStimulusAmplification(source.getFloatField("amplification"));
        logging::info("Amplification, nA: " + std::to_string(getStimulusAmplitifacation()));
        loadSaturationParameterList(source);
    }

    void StimulusSaturation::broadcastParameterList() {
        Application& app = Application::getInstance();
        app.broadcastDouble(darkCurrent, 0);
        app.broadcastDouble(stimulusAmplification, 0);
        broadcastSaturationParameterList();
    }

    void StimulusSaturation::setParameter(const std::string &name, const void *pvalue) {
        if (name == "dark_current") {
            setDarkCurrent(*(double *) pvalue);
        } else if (name == "amplification") {
            setStimulusAmplification(*(double*)pvalue);
        } else {
            setSaturationParameter(name, pvalue);
        }
    }

    void StimulusSaturation::initialize() {
        auto& stimulus = Application::getInstance().getStimulus();
        if (getInputProcessorNumber() == 0){
            addInputProcessor(&stimulus);
        } else if (getInputProcessorNumber() == 1) {
            throw wrong_input();
        }

        output = new data::LocalMatrix(getCommunicator(), stimulus.getGridX(), stimulus.getGridY(),
                stimulus.getSizeX(), stimulus.getSizeY(), 0.0);
    }

    void StimulusSaturation::update(double time) {
        auto pix = getOutput().begin();
        auto* input = *inputProcessorBegin();
        auto pix0 = input->getOutput().cbegin();

        for (; pix != getOutput().end(); ++pix, ++pix0){
            double unsaturated = getDarkCurrent() + getStimulusAmplitifacation() * *pix0;
            *pix = getSaturationOutput(unsaturated);
        }
    }

    void StimulusSaturation::finalizeProcessor(bool destruct) noexcept {

    }
}