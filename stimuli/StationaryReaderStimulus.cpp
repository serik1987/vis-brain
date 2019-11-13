//
// Created by serik1987 on 12.11.2019.
//

#include "StationaryReaderStimulus.h"
#include "../Application.h"
#include "../data/reader/BinReader.h"
#include "../log/output.h"

namespace stim{

    void StationaryReaderStimulus::loadStationaryStimulusParameters(const param::Object &source) {
        setFilename(source.getStringField("filename"));
        logging::info("Filename: " + getFilename());
    }

    void StationaryReaderStimulus::broadcastStationaryStimulusParameters() {
        Application& app = Application::getInstance();
        app.broadcastString(filename, 0);
    }

    void StationaryReaderStimulus::setStationaryStimulusParameter(const std::string &name, const void *pvalue) {
        setFilename((const char*)pvalue);
    }

    void StationaryReaderStimulus::fillStimulusMatrix() {
        data::reader::BinReader reader(getFilename());
        data::LocalMatrix* source = nullptr;
        try {
            source = reader.load(getCommunicator());
        } catch (std::exception& e){
            throw reading_failed();
        }
        if (source->getWidth() != stimulusMatrix->getWidth() || source->getHeight() != stimulusMatrix->getHeight()){
            delete source;
            throw reader_dimensions_mismatch();
        }

        double min_old = source->min();
        double max_old = source->max();
        double min_new = getLuminance() - 0.5 * getContrast();
        double max_new = getLuminance() + 0.5 * getContrast();
        double alpha = (max_new - min_new)/(max_old-min_old);
        double beta = min_new - alpha * min_old;

        auto pix_new = stimulusMatrix->begin();
        auto pix_old = source->cbegin();
        for (; pix_new != stimulusMatrix->end(); ++pix_new, ++pix_old){
            *pix_new = alpha * *pix_old + beta;
        }

        delete source;
    }
}