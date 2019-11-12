//
// Created by serik1987 on 11.11.2019.
//

#include "Processor.h"
#include "../log/output.h"

namespace equ{

    void Processor::finalize(bool destruct) noexcept{
        delete output;
        output = nullptr;
        if (!destruct) {
            finalizeProcessor();
        }
    }

    void Processor::addInputProcessor(Processor *pother) {
        inputProcessors.push_back(pother);
        pother->outputProcessor = this;
    }

    void Processor::removeInputProcessor(Processor *pother){
        inputProcessors.remove(pother);
        pother->outputProcessor = nullptr;
    }

    void Processor::printAllProcessors(int level, int root) {
        if (comm.getRank() == root){
            if (level == 0){
                std::cout << "\n";
            } else {
                for (int i = 0; i < level; i++) {
                    std::cout << "\t";
                }
            }
            std::cout << getName() << std::endl;
            for (Processor* input: inputProcessors){
                input->printAllProcessors(level+1, root);
            }
        }
    }

    Processor* Processor::createProcessor(mpi::Communicator& comm, const std::string& mechanism){
        using std::string;
        Processor* processor = nullptr;

        int delimiter = mechanism.find(':');
        if (delimiter == string::npos){
            throw param::WrongMechanism();
        }
        string mechanismClass = mechanism.substr(0, delimiter);
        string mechanismName = mechanism.substr(delimiter+1);

        if (mechanismClass == "stimulus"){
            processor = stim::Stimulus::createStimulus(comm, mechanismName);
        } else {
            throw param::UnknownMechanism(mechanism);
        }

        return processor;
    }

}