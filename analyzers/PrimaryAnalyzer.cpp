//
// Created by serik1987 on 01.12.2019.
//

#include "PrimaryAnalyzer.h"
#include "PrimaryVsdAnalyzer.h"

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
}