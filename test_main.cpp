//
// Created by serik1987 on 21.09.19.
//

#include "processors/Processor.h"
#include "models/abstract/glm/StimulusSaturation.h"
#include "models/abstract/glm/OdeTemporalKernel.h"
#include "models/abstract/glm/GaussianSpatialKernel.h"
#include "models/abstract/glm/DogFilter.h"
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

    logging::progress(0, 1, "Test of spatial convolution");

    std::string saturation_mechanism;
    std::string temporal_kernel_mechanism;
    std::string spatial_kernel_mechanism;
    std::string dog_filter_mechanism;
    if (comm.getRank() == 0){
        /* Dangerous and erroneous block of the code. This block is to remove after debugging will be completed */
        const param::Object& root = app.getParameterEngine().getRoot();
        param::Object parameter = app.getParameterEngine().getRoot().getObjectField("test_field");
        saturation_mechanism = parameter.getStringField("mechanism");
        temporal_kernel_mechanism = root.getObjectField("test_field_2").getStringField("mechanism");
        spatial_kernel_mechanism = root.getObjectField("test_field_3").getStringField("mechanism");
        dog_filter_mechanism = root.getObjectField("test_field_4").getStringField("mechanism");
        /* End of the dangerous block */
    }
    app.broadcastString(saturation_mechanism, 0);
    app.broadcastString(temporal_kernel_mechanism, 0);
    app.broadcastString(spatial_kernel_mechanism, 0);
    app.broadcastString(dog_filter_mechanism, 0);

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

    auto* temporal_kernel_inhibitory_processor = equ::Processor::createProcessor(comm, temporal_kernel_mechanism, parameters);
    auto* temporal_kernel_inhibitory = dynamic_cast<equ::OdeTemporalKernel*>(temporal_kernel_inhibitory_processor);
    temporal_kernel_inhibitory->setStimulusSaturation(saturation);

    auto* spatial_kernel_processor = equ::Processor::createProcessor(comm, spatial_kernel_mechanism, parameters);
    auto* spatial_kernel = dynamic_cast<equ::GaussianSpatialKernel*>(spatial_kernel_processor);
    spatial_kernel->setTemporalKernel(temporal_kernel);

    auto* spatial_kernel_inhibitory_processor = equ::Processor::createProcessor(comm, spatial_kernel_mechanism, parameters);
    auto* spatial_kernel_inhibitory = dynamic_cast<equ::GaussianSpatialKernel*>(spatial_kernel_inhibitory_processor);
    spatial_kernel_inhibitory->setTemporalKernel(temporal_kernel_inhibitory);

    auto* dog_filter_processor = equ::Processor::createProcessor(comm, dog_filter_mechanism, parameters);
    auto* dog_filter = dynamic_cast<equ::DogFilter*>(dog_filter_processor);
    dog_filter->setSpatialKernels(spatial_kernel, spatial_kernel_inhibitory);

    if (comm.getRank() == 0){
        param::Object source = app.getParameterEngine().getRoot().getObjectField("test_field");
        param::Object source2 = app.getParameterEngine().getRoot().getObjectField("test_field_2");
        param::Object source3 = app.getParameterEngine().getRoot().getObjectField("test_field_3");
        param::Object source2i = app.getParameterEngine().getRoot().getObjectField("test_field_i2");
        param::Object source3i = app.getParameterEngine().getRoot().getObjectField("test_field_i3");
        param::Object source4 = app.getParameterEngine().getRoot().getObjectField("test_field_4");
        saturation->loadParameters(source);
        logging::info("Excitation info");
        temporal_kernel->loadParameters(source2);
        spatial_kernel->loadParameters(source3);
        logging::info("Inhibition info");
        temporal_kernel_inhibitory->loadParameters(source2i);
        spatial_kernel_inhibitory->loadParameters(source3i);
        dog_filter->loadParameters(source4);
    }
    saturation->broadcastParameters();
    temporal_kernel->broadcastParameters();
    spatial_kernel->broadcastParameters();
    temporal_kernel_inhibitory->broadcastParameters();
    spatial_kernel_inhibitory->broadcastParameters();
    dog_filter->broadcastParameters();

    stimulus.initialize();
    saturation->initialize();
    temporal_kernel->initialize();
    spatial_kernel->initialize();
    temporal_kernel_inhibitory->initialize();
    spatial_kernel_inhibitory->initialize();
    dog_filter->initialize();
    method.initialize(*temporal_kernel);
    method.initialize(*temporal_kernel_inhibitory);
    data::stream::BinStream stream(&dog_filter->getOutput(), "output.bin", data::stream::Stream::Write, srate);

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
    logging::debug("");
    logging::debug("Spatial kernel mechanism " + spatial_kernel_mechanism);
    logging::debug("Kernel radius: " + to_string(spatial_kernel->getRadius()));
    logging::debug("");
    logging::debug("Final mechanism: " + dog_filter_mechanism);
    logging::debug("Dark rate: " + std::to_string(dog_filter->getDarkRate()));
    logging::debug("Excitatory weight: " + std::to_string(dog_filter->getExcitatoryWeight()));
    logging::debug("Inhibitory weight: " + std::to_string(dog_filter->getInhibitoryWeight()));
    logging::debug("Threshold: " + std::to_string(dog_filter->getThreshold()));
    dog_filter->printAllProcessors(0, comm.getRank());
    logging::debug("");
    logging::exit();

    double start_time = MPI_Wtime();

    double time = 0.0;
    int N = (int)(stimulus.getRecordLength() / integration_step) / 1;
    if (N == 0) N = 1;
    for (int i=0; time < stimulus.getRecordLength(); ++i, time = integration_step*i){
        stimulus.update(time);
        saturation->update(time);
        method.update(*temporal_kernel, i);
        spatial_kernel->update(time);
        method.update(*temporal_kernel_inhibitory, i);
        spatial_kernel_inhibitory->update(time);
        dog_filter->update(time);
#if SAVE_OUTPUT == 1
        stream.write(&dog_filter->getOutput());
        if (i % 1 == 0){
            logging::progress(i/1, N);
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
    spatial_kernel->finalize();
    temporal_kernel_inhibitory->finalize();
    spatial_kernel_inhibitory->finalize();
    dog_filter->finalize();


    delete saturation;
    delete temporal_kernel;
    delete spatial_kernel;
    delete temporal_kernel_inhibitory;
    delete spatial_kernel_inhibitory;
    delete dog_filter;

    logging::progress(1, 1);
}