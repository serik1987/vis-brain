//
// Created by serik1987 on 30.11.2019.
//

#include "SingleRunJob.h"
#include "../log/output.h"
#include "../methods/EqualDistributor.h"
#include "../data/stream/BinStream.h"

namespace job{

    void SingleRunJob::start() {
        Application& app = Application::getInstance();
        mpi::Communicator& comm = getJobCommunicator();
        method::Method& method = app.getMethod();
        net::Brain& brain = app.getBrain();
        double integration_step = method.getIntegrationTime();

        logging::progress(0, 1, "Application of the network distributor");
        method::EqualDistributor distributor(comm, method);
        distributor.apply(brain);


        app.createStimulus(distributor.getStimulusCommunicator());
        if (comm.getRank() == 0){
            auto brain_parameters = app.getParameterEngine().getRoot().getObjectField("brain");
            brain.loadParameters(brain_parameters);
        }
        brain.broadcastParameters();


        logging::progress(0, 1, "Adding all processors to the brain state");
        equ::State state(comm, method.getSolutionParameters());
        brain.addProcessorsToState(state);

        logging::progress(0, 1, "Initializing the state");
        app.getStimulus().initialize();
        state.initialize();
        method.initialize(state);
        auto& output = brain.getLayerByFullName("lgn")->getOutputData()->getOutput();
        double srate = 1000.0 / method.getIntegrationTime();
        data::stream::BinStream stream(&output, getOutputFilePrefix() + ".bin", data::stream::Stream::Write, srate);
        double start_time = MPI_Wtime();

        unsigned long long timestamp = 0;
        double time = 0;
        auto& stimulus = app.getStimulus();
        auto N = (unsigned long long)(stimulus.getRecordLength() / integration_step);
        logging::progress(0, N, "Single-run simulation");
        for (; time < stimulus.getRecordLength(); ++timestamp, time = integration_step*timestamp){
            stimulus.update(time);
            method.update(state, timestamp);
            auto& output = brain.getLayerByFullName("lgn")->getOutputData()->getOutput();
            stream.write(&output);
            logging::progress(timestamp, N);
        }

        double finish_time = MPI_Wtime();
        double total_time = finish_time - start_time;
        logging::enter();
        logging::debug("Elapsed time: " + std::to_string(total_time));
        logging::exit();
        logging::progress(0, 1, "Finalizing the state");
        state.finalize();
        logging::progress(1, 1);
    }
}