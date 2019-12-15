//
// Created by serik1987 on 03.12.2019.
//

#include "VsdWriter.h"
#include "../log/output.h"

namespace analysis{

    void VsdWriter::loadAnalysisParameters(const param::Object &source) {
        setFilenamePrefix(source.getStringField("filename"));
    }

    void VsdWriter::broadcastAnalysisParameters() {
        Application::getInstance().broadcastString(filename_prefix, 0);
    }

    void VsdWriter::setAnalyzerParameters(const std::string &name, const void *pvalue) {
        if (name == "filename"){
            setFilenamePrefix((const char*)pvalue);
        } else {
            throw param::IncorrectParameterName(name, "VSD writer");
        }
    }

    void VsdWriter::initializeVsd() {
        double srate = 1000.0 / getAcquisitionStep();
        std::string filename = getCurrentJob()->getOutputFilePrefix() + "_" + getFilenamePrefix() + ".bin";
        stream = new data::stream::BinStream(&getSource(), filename, data::stream::Stream::Write, srate);
    }

    void VsdWriter::update(double time) {
        stream->write(&getSource());
    }

    void VsdWriter::finalizeProcessor(bool destruct) noexcept {
        delete stream;
        stream = nullptr;
    }
}