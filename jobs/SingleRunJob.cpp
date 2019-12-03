//
// Created by serik1987 on 30.11.2019.
//

#include "SingleRunJob.h"
#include "../log/output.h"
#include "../data/stream/BinStream.h"
#include "../analyzers/AnalysisBuilder.h"

namespace job{

    void SingleRunJob::start() {
        auto& app = Application::getInstance();
        int rank = app.getAppCommunicator().getRank();
        auto& method = app.getMethod();
        double integration_step = method.getIntegrationTime();

        app.createDistributor(getJobCommunicator(), app.getMethod());
        app.createStimulus(app.getDistributor().getStimulusCommunicator());
        app.getBrain().createProcessors();
        app.createState(getJobCommunicator());

        auto& state = app.getState();

        logging::progress(0, 1, "Initializing the state");
        app.getStimulus().initialize();
        state.initialize();
        method.initialize(state);
        initializeAnalyzers();
        double start_time = MPI_Wtime();

        unsigned long long timestamp = 0;
        double time = 0;
        auto& stimulus = app.getStimulus();
        auto N = (unsigned long long)(stimulus.getRecordLength() / integration_step);
        logging::progress(0, N, "Single-run simulation");
        for (; time < stimulus.getRecordLength(); ++timestamp, time = integration_step*timestamp){
            stimulus.update(time);
            method.update(state, timestamp);
            updateAnalyzers(time);
            logging::progress(timestamp, N);
            getJobCommunicator().barrier();
            if (app.getInterrupted()){
                throw sys::application_interrupted();
            }
        }

        double finish_time = MPI_Wtime();
        double total_time = finish_time - start_time;
        logging::enter();
        logging::debug("Elapsed time: " + std::to_string(total_time));
        logging::exit();
        logging::progress(0, 1, "Finalizing the state");
        state.finalize();
        finalizeAnalyzers();
    }
}