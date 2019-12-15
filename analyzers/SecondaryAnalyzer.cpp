//
// Created by serik1987 on 03.12.2019.
//

#include "SecondaryAnalyzer.h"
#include "VsdWriter.h"
#include "../log/output.h"

namespace analysis{

    SecondaryAnalyzer *
    SecondaryAnalyzer::createSecondaryAnalyzer(mpi::Communicator &comm, const std::string &mechanism) {
        SecondaryAnalyzer* analyzer = nullptr;

        if (mechanism == "vsd-writer"){
            analyzer = new VsdWriter(comm);
        } else {
            throw param::UnknownMechanism("analysis:secondary." + mechanism);
        }

        return analyzer;
    }

    void SecondaryAnalyzer::loadSource() {
        using std::string;
        auto sep = getSourceName().find('.');

        if (sep == string::npos){
            throw incorrect_source(getSourceName());
        }
        string major_name = getSourceName().substr(0, sep);
        string minor_name = getSourceName().substr(sep+1);
        if (major_name != "analysis"){
            throw incorrect_source(getSourceName());
        }
        source = getCurrentJob()->getAnalyzer(minor_name);
        if (!source->isInputAcceptable()){
            throw input_not_acceptable();
        }
        checkSource(source);
    }

    bool SecondaryAnalyzer::isReady(double time) {
        if (current_time < time){
            ready = source->isReady(time);
            current_time = time;
        }

        return ready;
    }
}