//
// Created by serik1987 on 21.09.19.
//

#include "processors/Processor.h"
#include "models/abstract/glm/StimulusSaturation.h"
#include "data/stream/BinStream.h"

void test_main(){
    using namespace std;

    Application& app = Application::getInstance();
    mpi::Communicator& comm = app.getAppCommunicator();
    app.createStimulus(comm);
    auto& stimulus = app.getStimulus();

    logging::progress(0, 1, "Test of stimulus saturation");

    std::string saturation_mechanism;
    if (comm.getRank() == 0){
        /* Dangerous and erroneous block of the code. This block is to remove after debugging will be completed */
        param::Object parameter = app.getParameterEngine().getRoot().getObjectField("test_field");
        saturation_mechanism = parameter.getStringField("mechanism");
        /* End of the dangerous block */
    }
    app.broadcastString(saturation_mechanism, 0);

    auto* saturation_processor = equ::Processor::createProcessor(comm, saturation_mechanism);
    auto* saturation = dynamic_cast<equ::StimulusSaturation*>(saturation_processor);

    if (comm.getRank() == 0){
        param::Object source = app.getParameterEngine().getRoot().getObjectField("test_field");
        saturation->loadParameters(source);
    }
    saturation->broadcastParameters();

    logging::enter();
    logging::debug("Saturation mechanism selected: " + saturation_mechanism);
    if (saturation != nullptr) {
        logging::debug("Saturation name: " + saturation->getName());
        logging::debug("Dark current, nA: " + std::to_string(saturation->getDarkCurrent()));
        logging::debug("Stimulus amplification, nA: " + std::to_string(saturation->getStimulusAmplitifacation()));
        logging::debug("Maximum current, nA: " + std::to_string(saturation->getMaxCurrent()));
    } else {
        logging::debug("NULL WAS CREATED OR THE PROCESSOR IS NOT A STIMULUS SATURATION");
    }
    logging::debug("Number of input processors: " + std::to_string(saturation->getInputProcessorNumber()));
    logging::debug("");
    logging::exit();

    stimulus.initialize();
    saturation->initialize();
    data::stream::BinStream stream(&saturation->getOutput(), "output.bin", data::stream::Stream::Write, 100.0);


    logging::enter();
    logging::debug("Number of input processors: " + std::to_string(saturation->getInputProcessorNumber()));
    logging::exit();

    double time = 0.0;
    for (int i=0; time < stimulus.getRecordLength(); ++i){
        stimulus.update(time);
        saturation->update(time);
        stream.write(&saturation->getOutput());
        time = 10.0*i;
    }

    stimulus.finalize();
    saturation->finalize();

    logging::enter();
    logging::debug("Number of input processors: " + std::to_string(saturation->getInputProcessorNumber()));
    logging::exit();

    delete saturation;

    logging::progress(1, 1);
}