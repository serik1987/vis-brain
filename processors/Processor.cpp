//
// Created by serik1987 on 11.11.2019.
//

#include "Processor.h"
#include "../models/abstract/glm/GlmLayer.h"
#include "../log/output.h"
#include "../sys/auxiliary.h"

namespace equ{

    int Processor::idCounter = 0;

    void Processor::finalize(bool destruct) noexcept{
        if (!destruct) {
            finalizeProcessor();
        }
        delete output;
        output = nullptr;
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

    Processor* Processor::createProcessor(mpi::Communicator& comm, const std::string& mechanism,
            equ::Ode::SolutionParameters parameters){
        using std::string;
        Processor* processor = nullptr;

        int delimiter = mechanism.find(':');
        if (delimiter == string::npos){
            throw param::WrongMechanism();
        }
        string mechanismClass = mechanism.substr(0, delimiter);
        string mechanismName = mechanism.substr(delimiter+1);

        if (mechanismClass == "stimulus") {
            processor = stim::Stimulus::createStimulus(comm, mechanismName);
        } else if (mechanismClass == "glm") {
            processor = net::GlmLayer::createGlmMechanism(comm, mechanismName, parameters);
        } else {
            throw param::UnknownMechanism(mechanism);
        }

        return processor;
    }

    std::string Processor::lookExternalMechanism(const std::string &category, const std::string &external_name) {
        using sys::file_exists;
        std::string lib_path;
        std::string public_file = "/usr/local/lib/vis-brain/" + category + "/" + external_name + ".so";
        std::string private_file = sys::get_home_directory() + "/.vis-brain/" + category + "/" + external_name + ".so";
        std::string current_file = sys::get_current_directory() + "/" + category + "/" + external_name + ".so";

        if (file_exists(public_file)){
            lib_path = public_file;
        } else if (file_exists(private_file)) {
            lib_path = private_file;
        } else if (file_exists(current_file)){
            lib_path = current_file;
        } else {
            throw param::UnknownMechanism("external stimulus mechanism '" + external_name + "'");
        }

        return lib_path;
    }

}