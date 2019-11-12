//
// Created by serik1987 on 28.08.19.
//

#include <sstream>
#include <iomanip>
#include "compile_options.h"
#include <unistd.h>
#include "Application.h"
#include "log/output.h"
#include "compile_options.h"
#include "exceptions.h"
#include "param/Engine.h"
#include "param/exceptions.h"
#include "sys/Folder.h"
#include "sys/auxiliary.h"

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
    delete gen;
    delete log;
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
    std::cerr << "Output folder: " << output_folder << std::endl;
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

void Application::setParameter(const std::string &name, void *pvalue) {
    using std::string;

    auto pos = name.find('.');
    if (pos == string::npos){
        throw param::IncorrectParameterName(name, "world");
    }
    string parameter_class = name.substr(0, pos);
    string parameter_name = name.substr(pos+1);
    if (parameter_class == "application"){
        throw param::SetApplicationParameterError();
    } else {
        throw param::IncorrectParameterName(name, "world");
    }
}





void Application::simulate() {
    try {
        createStimulus(getAppCommunicator());
#if DEBUG==1
        test_main();
#endif
    } catch (std::exception& e){
        log->fail(e);
    }
}

void Application::createStimulus(mpi::Communicator& comm){
    mpi::Communicator& appComm = getAppCommunicator();
    std::string mechanism;
    int errcode = 0;
    logging::progress(0, 1, "Loading stimulus");

    if (appComm.getRank() == 0){
        try {
            auto &world = engine->getRoot();
            param::Object par_stimulus = world.getObjectField("stimulus");
            mechanism = par_stimulus.getStringField("mechanism");
        } catch (std::exception& exc){
            int local_errcode = -1;
            broadcastInteger(local_errcode, 0);
            throw;
        }
    }
    broadcastInteger(errcode, 0);
    if (errcode != 0){
        throw param::root_error();
    }
    broadcastString(mechanism, 0);
    logging::info("The following application stimulus was created");
    equ::Processor* proc = equ::Processor::createProcessor(comm, mechanism);
    stimulus = dynamic_cast<stim::Stimulus*>(proc);
    if (stimulus == nullptr){
        throw WrongStimulus();
    }

    if (appComm.getRank() == 0){
        auto &world = engine->getRoot();
        param::Object par_stimulus = world.getObjectField("stimulus");
        stimulus->loadParameters(par_stimulus);
    }
    stimulus->broadcastParameters();

    stimulus->initialize();
}