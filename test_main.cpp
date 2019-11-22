//
// Created by serik1987 on 21.09.19.
//

#include "processors/Processor.h"
#include "models/abstract/glm/StimulusSaturation.h"
#include "models/abstract/glm/OdeTemporalKernel.h"
#include "methods/ExplicitRecountEuler.h"
#include "methods/ExplicitEuler.h"
#include "methods/KhoinMethod.h"
#include "methods/ExplicitRungeKutta.h"
#include "data/stream/BinStream.h"

void test_main(){
    using namespace std;

    Application& app = Application::getInstance();
    mpi::Communicator& comm = app.getAppCommunicator();
    app.createStimulus(comm);
    auto& stimulus = app.getStimulus();

    logging::progress(0, 1, "Test of stimulus saturation");

    std::string saturation_mechanism;
    std::string temporal_kernel_mechanism;
    if (comm.getRank() == 0){
        /* Dangerous and erroneous block of the code. This block is to remove after debugging will be completed */
        const param::Object& root = app.getParameterEngine().getRoot();
        param::Object parameter = app.getParameterEngine().getRoot().getObjectField("test_field");
        saturation_mechanism = parameter.getStringField("mechanism");
        temporal_kernel_mechanism = root.getObjectField("test_field_2").getStringField("mechanism");
        /* End of the dangerous block */
    }
    app.broadcastString(saturation_mechanism, 0);
    app.broadcastString(temporal_kernel_mechanism, 0);

#define SAVE_OUTPUT 1
    constexpr double integration_step = 1.0;
    constexpr double srate = 1000.0 / integration_step;
    method::ExplicitRecountEuler method(integration_step);
    auto parameters = method.getSolutionParameters();

    auto* saturation_processor = equ::Processor::createProcessor(comm, saturation_mechanism, parameters);
    auto* saturation = dynamic_cast<equ::StimulusSaturation*>(saturation_processor);

    auto* temporal_kernel_processor = equ::Processor::createProcessor(comm, temporal_kernel_mechanism, parameters);
    auto* temporal_kernel = dynamic_cast<equ::OdeTemporalKernel*>(temporal_kernel_processor);
    temporal_kernel->setStimulusSaturation(saturation);


    if (comm.getRank() == 0){
        param::Object source = app.getParameterEngine().getRoot().getObjectField("test_field");
        param::Object source2 = app.getParameterEngine().getRoot().getObjectField("test_field_2");
        saturation->loadParameters(source);
        temporal_kernel->loadParameters(source2);
    }
    saturation->broadcastParameters();
    temporal_kernel->broadcastParameters();

    stimulus.initialize();
    saturation->initialize();
    temporal_kernel->initialize();
    method.initialize(*temporal_kernel);
    data::stream::BinStream stream(&temporal_kernel->getOutput(), "output.bin", data::stream::Stream::Write, srate);

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
    logging::debug("Initial solution parameters:");
    logging::debug("Time constant, steps: " + to_string(parameters.getTimeConstant()));
    logging::debug("Integration step, ms: " + to_string(parameters.getIntegrationStep()));
    logging::debug("Equation order: " + to_string(parameters.getEquationOrder()));
    logging::debug("Derivative order: " + to_string(parameters.getDerivativeOrder()));
    logging::debug("Double buffer mode: " + to_string(parameters.isDoubleBuffer()));
    logging::debug("Equation number: " + to_string(parameters.getEquationNumber()));
    logging::debug("");
    logging::debug("Solution parameters within the temporal kernel: ");
    auto& p = temporal_kernel->getSolutionParameters();
    logging::debug("Time constant, steps: " + to_string(p.getTimeConstant()));
    logging::debug("Integration step, ms: " + to_string(p.getIntegrationStep()));
    logging::debug("Equation order: " + to_string(p.getEquationOrder()));
    logging::debug("Derivative order: " + to_string(p.getDerivativeOrder()));
    logging::debug("Double buffer mode: " + to_string(p.isDoubleBuffer()));
    logging::debug("Equation number: " + to_string(p.getEquationNumber()));
    logging::debug("");
    logging::debug("Temporal kernel mechanism " + temporal_kernel_mechanism);
    logging::debug("Time constant: " + std::to_string(temporal_kernel->getTimeConstant()));
    temporal_kernel->printAllProcessors(0, comm.getRank());
    logging::exit();

    double start_time = MPI_Wtime();

    double time = 0.0;
    int N = (int)(stimulus.getRecordLength() / integration_step) / 10000;
    if (N == 0) N = 1;
    for (int i=0; time < stimulus.getRecordLength(); ++i, time = integration_step*i){
        stimulus.update(time);
        saturation->update(time);
        method.update(*temporal_kernel, i);
#if SAVE_OUTPUT == 1
        stream.write(&temporal_kernel->getOutput());
        if (i % 10000 == 0){
            logging::progress(i/10000, N);
        }
#endif
    }

    double finish_time = MPI_Wtime();

    logging::enter();
    logging::debug("Time spent for the integration processes: " + std::to_string(finish_time - start_time));
    logging::exit();

    stimulus.finalize();
    saturation->finalize();
    temporal_kernel->finalize();

    logging::enter();
    logging::debug("Number of input processors: " + std::to_string(saturation->getInputProcessorNumber()));
    logging::exit();

    delete saturation;
    delete temporal_kernel;

    logging::progress(1, 1);
}