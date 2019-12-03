//
// Created by serik1987 on 01.12.2019.
//

#include "PrimaryAnalyzer.h"
#include "PrimaryVsdAnalyzer.h"
#include "../Application.h"

namespace analysis{

    PrimaryAnalyzer *PrimaryAnalyzer::createPrimaryAnalyzer(mpi::Communicator &comm, const std::string& mechanism) {
        PrimaryAnalyzer* analyzer = nullptr;

        if (mechanism == "vsd"){
            analyzer = new PrimaryVsdAnalyzer(comm);
        } else {
            throw param::UnknownMechanism("analysis:primary." + mechanism);
        }

        return analyzer;
    }

    void PrimaryAnalyzer::loadSource() {
        using std::string;
        auto separator = getSourceName().find('.');
        if (separator == string::npos){
            throw invalid_analyzer_source();
        }

        std::string major_name = getSourceName().substr(0, separator);
        std::string minor_name = getSourceName().substr(separator+1);
        if (major_name != "brain"){
            throw invalid_analyzer_source();
        }

        source = Application::getInstance().getBrain().getLayerByFullName(minor_name);
    }
}