//
// Created by serik1987 on 01.12.2019.
//

#include "Analyzer.h"
#include "PrimaryAnalyzer.h"
#include "SecondaryAnalyzer.h"
#include "../log/output.h"

namespace analysis {

    Analyzer *Analyzer::createAnalyzer(mpi::Communicator &comm, const std::string &mechanism) {
        using std::string;
        Analyzer* analyzer = nullptr;

        auto dot = mechanism.find('.');
        string major_name;
        string minor_name;
        if (dot == string::npos){
            major_name = mechanism;
        } else {
            major_name = mechanism.substr(0, dot);
            minor_name = mechanism.substr(dot+1);
        }

        if (major_name == "primary") {
            analyzer = PrimaryAnalyzer::createPrimaryAnalyzer(comm, minor_name);
        } else if (major_name == "secondary"){
            analyzer = SecondaryAnalyzer::createSecondaryAnalyzer(comm, minor_name);
        } else {
            throw param::UnknownMechanism("analysis:" + mechanism);
        }

        return analyzer;
    }

    void Analyzer::loadParameterList(const param::Object &source) {
        setSourceName(source.getStringField("source"));
        loadSource();
        loadAnalysisParameters(source);
    }

    void Analyzer::broadcastParameterList() {
        Application& app = Application::getInstance();
        app.broadcastString(source_name, 0);
        if (app.getAppCommunicator().getRank() != 0){
            loadSource();
        }
        broadcastAnalysisParameters();
    }

    void Analyzer::setParameter(const std::string &name, const void *pvalue) {
        if (false) {

        } else {
            setAnalyzerParameters(name, pvalue);
        }
    }

    void Analyzer::initialize() {
        c = &getInputCommunicator();
        initializeAnalyzer();
    }
}