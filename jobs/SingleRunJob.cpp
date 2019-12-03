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
        auto& brain = app.getBrain();
        auto& method = app.getMethod();
        double integration_step = method.getIntegrationTime();

        app.createDistributor(getJobCommunicator(), app.getMethod());
        app.createStimulus(app.getDistributor().getStimulusCommunicator());
        app.getBrain().createProcessors();

        /* Start of the bad code */
        analysis::AnalysisBuilder builder(getJobCommunicator());
        if (app.getAppCommunicator().getRank() == 0){
            auto source = app.getParameterEngine().getRoot().getObjectField("job").getObjectField("analysis").getObjectField("vsd");
            builder.loadParameters(source);
        }
        builder.broadcastParameters();
        auto* primary_analyzer = builder.getAnalyzer();
        /* Finish of the bad code */

        app.createState(getJobCommunicator());

        auto& state = app.getState();

        logging::progress(0, 1, "Initializing the state");
        app.getStimulus().initialize();
        state.initialize();
        method.initialize(state);
        primary_analyzer->initialize();
        auto& output = primary_analyzer->getOutput();
        double srate = 1000.0 / method.getIntegrationTime();
        data::stream::BinStream stream(&output, getOutputFilePrefix() + ".bin", data::stream::Stream::Write, srate / 2);
        double start_time = MPI_Wtime();

        unsigned long long timestamp = 0;
        double time = 0;
        auto& stimulus = app.getStimulus();
        auto N = (unsigned long long)(stimulus.getRecordLength() / integration_step);
        logging::progress(0, N, "Single-run simulation");
        for (; time < stimulus.getRecordLength(); ++timestamp, time = integration_step*timestamp){
            stimulus.update(time);
            method.update(state, timestamp);
            if (primary_analyzer->isReady(time)){
                primary_analyzer->update(time);
                auto& output = primary_analyzer->getOutput();
                stream.write(&output);
            }
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
        primary_analyzer->finalize();

        delete primary_analyzer;
    }
}