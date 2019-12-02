/************************************************************************************************/
/*                                                                                              */
/* (C) Sergei Kozhukhov, a scientist in the Physiology of Sensory Systems Lab.,                 */
/*     the Institute of Higher Nervous Activity, Russian Academy of Sciences                    */
/* (C) the Institute of Higher Nervous Activity, Russian Academy of Sciences                    */
/* Correspondence address: 5A Butlerova str., 117485 Moscow, Russia                             */
/* E-mail: serik1987@gmail.com                                                                  */
/* Phone/What's App: +7 916 492 11 21                                                           */
/*                                                                                              */
/************************************************************************************************/

#include <sstream>
#include <iomanip>
#include <unistd.h>
#include "compile_options.h"
#include "Application.h"
#include "log/output.h"
#include "compile_options.h"
#include "exceptions.h"
#include "param/Engine.h"
#include "param/exceptions.h"
#include "sys/Folder.h"
#include "sys/auxiliary.h"
#include "stimuli/StimulusBuilder.h"
#include "methods/MethodBuilder.h"
#include "jobs/JobBuilder.h"
#include "methods/DistributorBuilder.h"

#if DEBUG==1
#include "test_main.cpp"

#endif

Application* Application::instance = nullptr;

Application::Application(int* argc, char ***argv):
    mpi::App(argc, argv, APP_NAME),
    application_ready(false){
    if (instance != nullptr){
        throw ApplicationAlreadyExists();
    }
    instance = this;
    argument_number = *argc;
    argument_pointer = *argv;
}

void Application::deleteV8Engine() {
    if (engine != nullptr){
        delete engine;
        engine = nullptr;
    }
}

void Application::clearEngines(){
    delete stimulus;
    stimulus = nullptr;
    delete gen;
    gen = nullptr;
    delete log;
    log = nullptr;
    delete method;
    method = nullptr;
    delete brain;
    brain = nullptr;
    delete distributor;
    distributor = nullptr;
    delete state;
    state = nullptr;
    deleteV8Engine();
}

Application::~Application(){
    clearEngines();
}


void Application::loadAllParameters() {
    mpi::Communicator& comm = getAppCommunicator();
    if (comm.getRank() == 0){
        engine = new param::Engine(&argument_number, &argument_pointer);
        const param::Object& world = engine->getRoot();
        is_gui = engine->getGui();
        loadParameters(world.getObjectField("application"));
        if (is_gui){
            std::cout << world.stringify() << std::endl;
            std::cerr << "Model parameters were sent to the output stream. Simulation aborted\n";
        }
    }
    broadcastParameters();
    if (process_number == comm.getProcessorNumber() && !is_gui){
        log = new logging::Engine(output_folder);
#if AUTO_INITIALIZATION==1
        getNoiseEngine();
#endif
    }
}


void Application::loadParameterList(const param::Object &source) {
    std::cerr << "Loading application parameters...";
    mpi::Communicator& comm = getAppCommunicator();
    parent = source.getStringField("parent");
    process_number = source.getIntegerField("process_number");
    if (process_number <= 0){
        throw WrongProcessorNumber();
    }
    std::string mode = source.getStringField("configuration_mode");
    if (mode == "simple"){
        configuration_mode = Simple;
    } else if (mode == "external"){
        configuration_mode = External;
        host_configuration_file = source.getStringField("source");
    } else {
        throw UnknownMpiConfiguration();
    }
    if (process_number != comm.getProcessorNumber() && comm.getRank() == 0 && !is_gui){
        fillCmd();
    }
    std::cerr << "\033[32;1mOK\033[0m\n";
    if (process_number != comm.getProcessorNumber() || is_gui) return;
    setOutputFolder(source.getStringField("output_folder_prefix"));
}

void Application::setOutputFolder(const std::string &folder_prefix) {
    using std::string;
    output_folder = folder_prefix;
    int folder_number = 0;
    string name_start = folder_prefix + "_";
    for (auto info: sys::Folder(".")){
        if (info.d_type == DT_DIR){
            string dir_name = info.d_name;
            if (dir_name.find(name_start) == 0){
                int iStart = name_start.length();
                int iFinish = dir_name.find("_", iStart);
                string simulation_number = dir_name.substr(iStart, iFinish - iStart);
                int current_folder_number = atoi(simulation_number.c_str());
                if (current_folder_number >= folder_number){
                    folder_number = current_folder_number + 1;
                }
            }
        }
    }
    std::ostringstream output_folder_stream;
    output_folder_stream << name_start << std::setw(3) << std::setfill('0') << folder_number << "_" << getDate();
    output_folder = output_folder_stream.str();
    sys::create_empty_dir(output_folder);
    std::cout << "Output folder: " << output_folder << std::endl;
}

std::string Application::getDate(){
    time_t now = ::time(0);
    tm* t = localtime(&now);
    std::ostringstream s;
    s << t->tm_year+1900 << std::setw(2) << std::setfill('0') << t->tm_mon+1 <<
        std::setw(2) << std::setfill('0') << t->tm_mday;
    return s.str();
}

void Application::broadcastParameterList(){
    broadcastBoolean(is_gui, 0);
    broadcastString(parent, 0);
    broadcastInteger(process_number, 0);
    int configuration_mode_buffer = configuration_mode;
    broadcastInteger(configuration_mode_buffer, 0);
    configuration_mode = (ConfigurationMode)configuration_mode_buffer;
    switch (configuration_mode){
        case Simple:
            break;
        case External:
            broadcastString(host_configuration_file, 0);
            break;
        default:
            throw UnknownMpiConfiguration();
    }
    if (process_number != getAppCommunicator().getProcessorNumber() || is_gui) return;
    broadcastString(output_folder, 0);
    application_ready = true;
}

void Application::fillCmd() {
    std::stringstream buf;
    buf << parent << " ";
    switch (configuration_mode){
        case Simple:
            buf << "-n " << process_number << " " << APP_NAME;
            break;
        case External:
            buf << "-n " << process_number << " -f " << host_configuration_file << " " << APP_NAME;
            break;
        default:
            throw UnknownMpiConfiguration();
    }
    for (int i=1; i < argument_number; i++){
        buf << " \"" << argument_pointer[i] << "\"";
    }
    cmd = buf.str();
}

void Application::setParameter(const std::string &name, const void *pvalue) {
    using std::string;

    auto pos = name.find('.');
    if (pos == string::npos){
        throw param::IncorrectParameterName(name, "world");
    }
    string parameter_class = name.substr(0, pos);
    string parameter_name = name.substr(pos+1);
    if (parameter_class == "application") {
        throw param::SetApplicationParameterError();
    } else if (parameter_class == "stimulus"){
        stimulus->setParameter(parameter_name, pvalue);
    } else {
        throw param::IncorrectParameterName(name, "world");
    }
}





void Application::simulate() {
    try {
#if TEST_MODE==1
        test_main();
#else
        sys::set_signal_processors();
        createMethod();
        createBrain();
        job::Job* mainJob = getMainJob();
        mainJob->start();
        delete mainJob;
        logging::progress(1, 1);
#endif
    } catch (std::exception& e){
        log->fail(e);
    }
}

void Application::createStimulus(mpi::Communicator& comm){
    logging::info("");
    logging::info("Stimulus information");
    stim::StimulusBuilder builder(comm);
    if (getAppCommunicator().getRank() == 0){
        auto source = engine->getRoot().getObjectField("stimulus");
        builder.loadParameters(source);
    }
    builder.broadcastParameters();
    stimulus = builder.getStimulus();
}

void Application::createMethod() {
    logging::progress(0, 1, "Defining integration method");
    method::MethodBuilder builder;
    if (getAppCommunicator().getRank() == 0){
        auto source = engine->getRoot().getObjectField("application");
        builder.loadParameters(source);
    }
    builder.broadcastParameters();
    method = builder.build();
    logging::info("");
}

void Application::createBrain() {
    brain = new net::Brain();
    if (getAppCommunicator().getRank() == 0){
        auto brain_parameters = getParameterEngine().getRoot().getObjectField("brain");
        brain->loadParameters(brain_parameters);
    }
    brain->broadcastParameters();
    logging::info("General network configuration");
    logging::info(brain->printNetworkConfiguration());
}

job::Job* Application::getMainJob() {
    job::Job* mainJob;
    logging::info("Job information");
    logging::progress(0, 1, "Loading job info");

    job::JobBuilder builder(getAppCommunicator());
    if (getAppCommunicator().getRank() == 0){
        auto job_parameters = getParameterEngine().getRoot().getObjectField("job");
        builder.loadParameters(job_parameters);
    }
    builder.broadcastParameters();
    mainJob = builder.getJob();

    return mainJob;

}

void Application::createDistributor(mpi::Communicator& comm, method::Method& method, bool apply) {
    method::DistributorBuilder builder(comm, method);
    if (getAppCommunicator().getRank() == 0){
        auto parameters = engine->getRoot().getObjectField("application");
        builder.loadParameters(parameters);
    }
    builder.broadcastParameters();
    distributor = builder.build();
    if (apply){
        distributor->apply(*brain);
    }
}

void Application::createState(mpi::Communicator& comm) {
    logging::progress(0, 1, "Adding all processors to the brain state");
    state = new equ::State(comm, getMethod().getSolutionParameters());
    brain->addProcessorsToState(*state);
}
