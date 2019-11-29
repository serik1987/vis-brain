//
// Created by serik1987 on 21.09.19.
//

#include "methods/ExplicitRecountEuler.h"
#include "models/Brain.h"
#include "methods/EqualDistributor.h"
#include "processors/State.h"
#include "data/stream/BinStream.h"

void test_main(){
    using namespace std;

    /* First of all, we shall define communicator. This is an example of a single-job run where communicator is
     * an application communicator and contains all processes */
    Application& app = Application::getInstance();
    mpi::Communicator& comm = app.getAppCommunicator();

    /*
     * Next, let's define the solution parameters. We need to tell two main parameters: certain solution method
     * and the integration step
     */
    constexpr double integration_step = 1.0;
    logging::progress(0, 1, "Defining integration method");
    method::ExplicitRecountEuler method(integration_step);

    /*
     * At third stage we shall create an empty brain.
     */
    net::Brain brain;

    /*
     * Let's load and broadcast all brain parameters
     */
    if (comm.getRank() == 0){
        auto brain_parameters = app.getParameterEngine().getRoot().getObjectField("brain");
        brain.loadParameters(brain_parameters);
    }
    brain.broadcastParameters();
    logging::info("General network configuration");
    logging::info(brain.printNetworkConfiguration());

    /*
     * After we created all layers and set up all connections between the layers, that's time to create the distibutors
     * for them
     */
    logging::progress(0, 1, "Application of the network distributor");
    method::EqualDistributor distributor(comm, method);
    distributor.apply(brain);

    /*
     * After all layers were distributed among the processes, the network parameters shall be loaded again
     * createStimulus method will load all stimulus parameters
     */
    app.createStimulus(distributor.getStimulusCommunicator());
    if (comm.getRank() == 0){
        auto brain_parameters = app.getParameterEngine().getRoot().getObjectField("brain");
        brain.loadParameters(brain_parameters);
    }
    brain.broadcastParameters();

    /*
     * Let's define the state and add all processors to the state
     */
    logging::progress(0, 1, "Adding all processors to the brain state");
    equ::State state(comm, method.getSolutionParameters());
    brain.addProcessorsToState(state);

    /*
     * Initialization
     */
    logging::progress(0, 1, "Initializing the state");
    app.getStimulus().initialize();
    state.initialize();
    method.initialize(state);
    auto& output = brain.getLayerByFullName("lgn")->getOutputData()->getOutput();
    constexpr double srate = 1000.0 / integration_step;
    data::stream::BinStream stream(&output, "output.bin", data::stream::Stream::Write, srate);
    double start_time = MPI_Wtime();

    /*
     * Simulation
     */
    unsigned long timestamp = 0;
    double time = 0;
    auto& stimulus = app.getStimulus();
    auto N = (unsigned long)(stimulus.getRecordLength() / integration_step);
    logging::progress(0, N, "Single-run simulation");
    for (; time < stimulus.getRecordLength(); ++timestamp, time = integration_step*timestamp){
        stimulus.update(time);
        method.update(state, timestamp);
        auto& output = brain.getLayerByFullName("lgn")->getOutputData()->getOutput();
        stream.write(&output);
        logging::progress(timestamp, N);
    }

    /*
     * Finalization
     */
    double finish_time = MPI_Wtime();
    double total_time = finish_time - start_time;
    logging::enter();
    logging::debug("Elapsed time: " + std::to_string(total_time));
    logging::exit();
    logging::progress(0, 1, "Finalizing the state");
    state.finalize();



    /*

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
        // Dangerous and erroneous block of the code. This block is to remove after debugging will be completed
        const param::Object& root = app.getParameterEngine().getRoot();
        param::Object parameter = app.getParameterEngine().getRoot().getObjectField("test_field");
        saturation_mechanism = parameter.getStringField("mechanism");
        temporal_kernel_mechanism = root.getObjectField("test_field_2").getStringField("mechanism");
        spatial_kernel_mechanism = root.getObjectField("test_field_3").getStringField("mechanism");
        dog_filter_mechanism = root.getObjectField("test_field_4").getStringField("mechanism");
        // End of the dangerous block
    }
    app.broadcastString(saturation_mechanism, 0);
    app.broadcastString(temporal_kernel_mechanism, 0);
    app.broadcastString(spatial_kernel_mechanism, 0);
    app.broadcastString(dog_filter_mechanism, 0);

#define SAVE_OUTPUT 1
    constexpr double integration_step = 1.0;
    constexpr double srate = 1000.0 / integration_step;
    method::ExplicitRecountEuler method(integration_step);
    equ::State state(comm, method.getSolutionParameters());

    auto* saturation_processor = state.createProcessor(saturation_mechanism);
    auto* saturation = dynamic_cast<equ::StimulusSaturation*>(saturation_processor);

    auto* temporal_kernel_processor = state.createProcessor(temporal_kernel_mechanism);
    auto* temporal_kernel = dynamic_cast<equ::OdeTemporalKernel*>(temporal_kernel_processor);
    temporal_kernel->setStimulusSaturation(saturation);

    auto* temporal_kernel_inhibitory_processor = state.createProcessor(temporal_kernel_mechanism);
    auto* temporal_kernel_inhibitory = dynamic_cast<equ::OdeTemporalKernel*>(temporal_kernel_inhibitory_processor);
    temporal_kernel_inhibitory->setStimulusSaturation(saturation);

    auto* spatial_kernel_processor = state.createProcessor(spatial_kernel_mechanism);
    auto* spatial_kernel = dynamic_cast<equ::GaussianSpatialKernel*>(spatial_kernel_processor);
    spatial_kernel->setTemporalKernel(temporal_kernel);

    auto* spatial_kernel_inhibitory_processor = state.createProcessor(spatial_kernel_mechanism);
    auto* spatial_kernel_inhibitory = dynamic_cast<equ::GaussianSpatialKernel*>(spatial_kernel_inhibitory_processor);
    spatial_kernel_inhibitory->setTemporalKernel(temporal_kernel_inhibitory);

    auto* dog_filter_processor = state.createProcessor(dog_filter_mechanism);
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

    state.addProcessor(dog_filter);
    state.printProcessorList();
    stimulus.initialize();
    state.initialize();
    method.initialize(state);

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
    logging::debug("Time constant, steps: " + to_string(state.getSolutionParameters().getTimeConstant()));
    logging::debug("Integration step, ms: " + to_string(state.getSolutionParameters().getIntegrationStep()));
    logging::debug("Equation order: " + to_string(state.getSolutionParameters().getEquationOrder()));
    logging::debug("Derivative order: " + to_string(state.getSolutionParameters().getDerivativeOrder()));
    logging::debug("Double buffer mode: " + to_string(state.getSolutionParameters().isDoubleBuffer()));
    logging::debug("Equation number: " + to_string(state.getSolutionParameters().getEquationNumber()));
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
        method.update(state, i);
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

    state.finalize();

    */

    logging::progress(1, 1);
}