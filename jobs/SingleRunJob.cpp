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

        delete primary_analyzer;
    }
}