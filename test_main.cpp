//
// Created by serik1987 on 21.09.19.
//

#include "methods/ExplicitRecountEuler.h"
#include "models/Brain.h"
#include "methods/EqualDistributor.h"
#include "processors/State.h"
#include "data/stream/BinStream.h"
#include "jobs/JobBuilder.h"

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
    app.setMethod(&method);

    /*
     * At third stage we shall create an empty brain.
     */
    net::Brain brain;
    app.setBrain(&brain);

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
     * Creating the launching the job
     */
    job::JobBuilder builder(comm);
    logging::info("Job information");
    logging::progress(0, 1, "Loading job info");
    if (comm.getRank() == 0){
        auto job_parameters = app.getParameterEngine().getRoot().getObjectField("job");
        builder.loadParameters(job_parameters);
    }
    builder.broadcastParameters();
    auto* job = builder.getJob();
    job->start();

    /* Final operations */
    logging::progress(1, 1);
    delete job;


    /*
    *
     * After we created all layers and set up all connections between the layers, that's time to create the distibutors
     * for them
     *

    */
}