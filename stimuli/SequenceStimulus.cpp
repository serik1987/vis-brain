//
// Created by serik1987 on 17.11.2019.
//

#include <algorithm>

#include "SequenceStimulus.h"
#include "../log/output.h"

namespace stim{

    void SequenceStimulus::loadComplexStimulusParameters(const param::Object &source) {
        logging::info("This complex stimulus is a stimulus sequence with the following parameters:");
        setShuffle(source.getBooleanField("shuffle"));
        if (isShuffle()){
            logging::info("Sequence shuffling ON");
        } else {
            logging::info("Sequence shuffling OFF");
        }
        setRepeats(source.getIntegerField("repeats"));
        logging::info("Number of repeats: " + std::to_string(getRepeats()));
        setName(source.getStringField("name"));
    }

    void SequenceStimulus::broadcastComplexStimulusParameters() {
        Application& app = Application::getInstance();
        app.broadcastBoolean(shuffle, 0);
        app.broadcastInteger(repeats, 0);
        app.broadcastString(name, 0);
    }

    void SequenceStimulus::setComplexStimulusParameter(const std::string &name, const void *pvalue) {
        if (name == "shuffle") {
            setShuffle(*(bool *) pvalue);
        } else if (name == "repeats") {
            setRepeats(*(int *) pvalue);
        } else if (name == "name"){
            setName((const char*)pvalue);
        } else {
            throw param::IncorrectParameterName(name, "sequence stimulus");
        }
    }

    void SequenceStimulus::initializeComplexStimulus() {
        Application& app = Application::getInstance();
        recordLength = 0.0;

        for (auto it = inputProcessorBegin(); it != inputProcessorEnd(); ++it){
            auto* stimulus = dynamic_cast<Stimulus*>(*it);
            if (stimulus == nullptr){
                throw WrongStimulus();
            }
            recordLength += stimulus->getRecordLength();
        }

        singleRepeatLength = recordLength;
        recordLength *= getRepeats();
        repeatStartTime = 0.0;
        frameNumber = 0;
        trialStartTime = 0.0;
        currentTrial = 0;
        repeatNumber = 0;

        indices = new std::vector<int>(getInputProcessorNumber());
        int idx = 0;
        for (auto& item: *indices){
            item = idx++;
        }

        if (isShuffle()){
            std::shuffle(indices->begin(), indices->end(), app.getNoiseEngine());
            int* buffer = &(*indices)[0];
            getCommunicator().broadcast(buffer, indices->size(), MPI_INT, 0);
        }

        currentStimulus = (*indices)[0];
        pstimulus = dynamic_cast<Stimulus*>(getInputProcessor(currentStimulus));
        trialLength = pstimulus->getRecordLength();

        printProtocolLine();
    }

    void SequenceStimulus::update(double time){
        Application& app = Application::getInstance();
        auto& engine = app.getNoiseEngine();
        double timeFromRepeatStart = time - repeatStartTime;
        if (timeFromRepeatStart >= singleRepeatLength){
            repeatStartTime += singleRepeatLength;
            timeFromRepeatStart -= singleRepeatLength;
            trialStartTime = 0.0;
            currentTrial = 0;
            if (isShuffle()){
                std::shuffle(indices->begin(), indices->end(), app.getNoiseEngine());
                int* buffer = &(*indices)[0];
                getCommunicator().broadcast(buffer, indices->size(), MPI_INT, 0);
            }
            currentStimulus = (*indices)[0];
            pstimulus = dynamic_cast<Stimulus*>(getInputProcessor(currentStimulus));
            trialLength = pstimulus->getRecordLength();
            ++repeatNumber;
            printProtocolLine();
        }

        double timeFromTrialStart = timeFromRepeatStart - trialStartTime;
        if (timeFromTrialStart >= trialLength){
            trialStartTime += trialLength;
            timeFromTrialStart -= trialLength;
            ++currentTrial;
            currentStimulus = (*indices)[currentTrial];
            pstimulus = dynamic_cast<Stimulus*>(getInputProcessor(currentStimulus));
            trialLength = pstimulus->getRecordLength();
            pstimulus->finalize(false);
            pstimulus->initialize();
            printProtocolLine();
        }

        logging::enter();
        logging::debug("UPDATE t = " + std::to_string(time) + " TIME FROM START = " + std::to_string(timeFromRepeatStart) +
            " FRAME NUMBER = " + std::to_string(frameNumber) + " REPEAT START TIME = " + std::to_string(repeatStartTime) +
            " TIME FROM TRIAL START = " + std::to_string(timeFromTrialStart) + " START TRIAL TIME = "
            + std::to_string(trialStartTime) + " TRIAL LENGTH = " + std::to_string(trialLength) +
            " CURRENT TRIAL = " + std::to_string(currentTrial) + " CURRENT STIMULUS = " + std::to_string(currentStimulus));
        std::stringstream ss;
        for (auto x: *indices){
            ss << x << "\t";
        }
        logging::debug(ss.str());
        logging::exit();

        pstimulus->update(timeFromTrialStart);
        auto pix = output->begin();
        auto pix0 = pstimulus->getOutput().begin();
        for (; pix != output->end(); ++pix, ++pix0){
            *pix = *pix0;
        }

        frameNumber++;
    }

    void SequenceStimulus::printProtocolLine(){
        if (isShuffle()){
            std::stringstream ss;
            ss << "STIMULATION PROTOCOL. SEQUENCE " << getName() << " REPEAT NUMBER " << repeatNumber <<
               " FRAME NUMBER " << frameNumber << " STIMULUS INDEX " << currentStimulus;
            logging::info(ss.str());
        }
    }

    void SequenceStimulus::finalizeComplexStimulus(bool destruct) {
        delete indices;
        indices = nullptr;

        logging::enter();
        logging::debug("FINALIZE");
        logging::exit();
    }

}