//
// Created by serik1987 on 14.11.2019.
//

#include "StreamStimulus.h"
#include "../log/output.h"

namespace stim{

    void StreamStimulus::loadMovingStimulusParameters(const param::Object &source) {
        setFilename(source.getStringField("filename"));
        logging::info("Filename: " + getFilename());
    }

    void StreamStimulus::broadcastMovingStimulusParameters() {
        Application& app = Application::getInstance();
        app.broadcastString(filename, 0);
    }

    void StreamStimulus::setMovingStimulusParameter(const std::string &name, const void *pvalue) {
        if (name == "filename"){
            setFilename((const char*)pvalue);
        } else {
            throw param::IncorrectParameterName(name, "stream stimulus");
        }
    }

    void StreamStimulus::initializeExtraBuffer() {
        try{
            using std::to_string;
            stream = new data::stream::BinStream(output, getFilename(), data::stream::Stream::Read, 1.0);
            streamFinished = false;
        } catch (const std::exception& e){
            throw stream_opening_failed(e);
        }
    }

    void StreamStimulus::updateMovingStimulus(double relativeTime) {
        using std::to_string;
        if (!streamFinished){
            double simulationTime = 0.001 * relativeTime; // relative time is in ms, simulation time is in s
            double srate = stream->getSampleRate();
            double streamTime = stream->getFrameNumber() / srate;
            bool framesUpdated = false;
            while (streamTime <= simulationTime){
                if (stream->getFrameNumber() >= stream->getTotalFrames()){
                    streamFinished = true;
                    logging::enter();
                    logging::warning("Stream duration is smaller than stimulus duration. End of stream has reached. "
                                     "Simulation will continue but the stimulus change is stopped");
                    logging::exit();
                    break;
                }
                stream->read();
                streamTime = stream->getFrameNumber() / srate;
                framesUpdated = true;
            }
            if (framesUpdated){
                for (auto pix = output->begin(); pix != output->end(); ++pix){
                    if (*pix < 0.0) *pix = 0.0;
                    if (*pix > 1.0) *pix = 1.0;
                }
            }
        }
    }

    void StreamStimulus::finalizeExtraBuffer(bool destruct) {
        if (stream != nullptr) {
            using std::to_string;
            logging::enter();
            if (stream->getFrameNumber() < stream->getTotalFrames()){
                logging::warning("Simulation completed but not all stream frames has read because stream duration is "
                                 "higher than the stimulus duration");
            }
            logging::exit();
        }
        delete stream;
        stream = nullptr;
    }
}