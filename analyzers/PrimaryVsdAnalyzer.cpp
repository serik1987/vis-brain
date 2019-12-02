//
// Created by serik1987 on 01.12.2019.
//

#include "PrimaryVsdAnalyzer.h"
#include "../log/output.h"

namespace analysis{

    void PrimaryVsdAnalyzer::loadAnalysisParameters(const param::Object &source) {
        std::cerr << "LOAD PRIMARY VSD ANALYSIS PARAMETERS\n";
    }

    void PrimaryVsdAnalyzer::broadcastAnalysisParameters() {
        logging::enter();
        logging::debug("BROADCAST VSD ANALYSIS PARAMETERS");
        logging::exit();
    }

    void PrimaryVsdAnalyzer::setAnalyzerParameters(const std::string &name, const void *pvalue) {
        if (false) {

        } else {
            throw param::IncorrectParameterName(name, "primary VSD analyser");
        }
    }

    void PrimaryVsdAnalyzer::initializeVsd() {
        logging::enter();
        logging::debug("INITIALIZATION OF THE PRIMARY VSD ANALYZER");
        logging::exit();
    }

    void PrimaryVsdAnalyzer::finalizeProcessor(bool destruct) noexcept {
        logging::enter();
        logging::debug("DESTRUCTION OF THE PRIMARY VSD ANALYZER " + std::to_string(destruct));
        logging::exit();
    }

    void PrimaryVsdAnalyzer::update(double time) {
        logging::enter();
        logging::debug("UPDATE PRIMARY VSD ANALYZER AT t = " + std::to_string(time));
        logging::exit();
    }
}