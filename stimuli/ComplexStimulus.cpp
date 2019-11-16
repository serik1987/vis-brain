//
// Created by serik1987 on 16.11.2019.
//

#include "ComplexStimulus.h"
#include "WeightedStimulus.h"
#include "BoundedStimulus.h"
#include "../log/output.h"

namespace stim {

    ComplexStimulus *ComplexStimulus::createComplexStimulus(mpi::Communicator &comm, const std::string &name) {
        ComplexStimulus* stimulus = nullptr;

        if (name == "weighted") {
            stimulus = new WeightedStimulus(comm);
        } else if (name == "bounded"){
            stimulus = new BoundedStimulus(comm);
        } else {
            throw param::UnknownMechanism("stimulus:complex." + name);
        }

        return stimulus;
    }

    void ComplexStimulus::loadStimulusParameters(const param::Object &source) {
        param::Object stimuli = source.getObjectField("content");
        uint32_t stim_number = stimuli.getPropertyNumber();
        mechanism_names.reserve(stim_number);
        logging::info("This is a complex stimulus that contains the following child stimuli:\n");

        for (uint32_t index = 0; index < stim_number; ++index){
            logging::info("Child stimulus # " + std::to_string(index+1));
            param::Object local_source = stimuli.getObjectField(index);
            std::string mechanism = local_source.getStringField("mechanism");
            mechanism_names.push_back(mechanism);
            Processor* processor = Processor::createProcessor(getCommunicator(), mechanism);
            auto* stimulus = dynamic_cast<Stimulus*>(processor);
            if (stimulus == nullptr){
                delete processor;
                throw WrongStimulus();
            }
            addInputProcessor(stimulus);
            stimulus->loadParameters(local_source);
            logging::info("\n");
            if (stimulus->getGridX() != getGridX() || stimulus->getGridY() != getGridY()){
                throw stimulus_dimensions_mismatch();
            }
            if (stimulus->getSizeX() != getSizeX() || stimulus->getSizeY() != getSizeY()){
                throw stimulus_size_mismatch();
            }
        }

        loadComplexStimulusParameters(source);
    }

    void ComplexStimulus::broadcastStimulusParameters() {
        Application& app = Application::getInstance();
        int rank = app.getAppCommunicator().getRank();
        auto iterator = inputProcessorBegin();
        int child_number = mechanism_names.size();
        app.broadcastInteger(child_number, 0);

        for (int index = 0; index < child_number; ++index){
            std::string mechanism_name;
            Stimulus* child_stimulus = nullptr;
            if (rank == 0){
                mechanism_name = mechanism_names[index];
                child_stimulus = dynamic_cast<Stimulus*>(*iterator);
                if (child_stimulus == nullptr){
                    throw WrongStimulus();
                }
            }
            app.broadcastString(mechanism_name, 0);
            if (rank != 0){
                mechanism_names.push_back(mechanism_name);
                child_stimulus = dynamic_cast<Stimulus*>(Processor::createProcessor(getCommunicator(), mechanism_name));
                addInputProcessor(child_stimulus);
            }

            child_stimulus->broadcastParameters();

            if (rank == 0){
                ++iterator;
            }
        }

        broadcastComplexStimulusParameters();
    }

    void ComplexStimulus::setStimulusParameter(const std::string &name, const void *pvalue) {
        using std::string;
        size_t delimiter = name.find('.');
        if (delimiter != string::npos){
            string sindex = name.substr(0, delimiter);
            int index = -1;
            try{
                index = std::stoi(sindex);
            } catch (std::invalid_argument& e){
                throw param::IncorrectParameterName(name, "complex stimulus");
            }
            string child_name = name.substr(delimiter + 1);
            Processor* proc = getInputProcessor(index);
            proc->setParameter(name, pvalue);
        } else {
            setComplexStimulusParameter(name, pvalue);
        }
    }

    void ComplexStimulus::initializeStimulus() {
        for (auto it = inputProcessorBegin(); it != inputProcessorEnd(); ++it){
            (*it)->initialize();
        }
        initializeComplexStimulus();
    }

    void ComplexStimulus::finalizeProcessor(bool destruct) noexcept {
        if (!destruct) {
            finalizeComplexStimulus(false);
        }

        for (auto it = inputProcessorBegin(); it != inputProcessorEnd(); ++it){
            (*it)->finalize(destruct);
        }

        if (destruct){
            for (auto it = inputProcessorBegin(); it != inputProcessorEnd(); ++it){
                delete *it;
            }
        }

        recordLength = 0.0;
    }
}