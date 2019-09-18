//
// Created by serik1987 on 28.08.19.
//

#include <sstream>
#include "Application.h"
#include "compile_options.h"
#include "exceptions.h"
#include "param/Engine.h"
#include "sys/Folder.h"

Application* Application::instance = nullptr;

Application::Application(int* argc, char ***argv):
    mpi::App(argc, argv, APP_NAME),
    application_ready(false){
    if (instance != nullptr){
        throw "application already exists";
    }
    instance = this;
    argument_number = *argc;
    argument_pointer = *argv;
}

Application::~Application(){
}


void Application::loadAllParameters() {
    mpi::Communicator& comm = getAppCommunicator();
    if (comm.getRank() == 0){
        param::Engine engine(&argument_number, &argument_pointer);
        const param::Object& world = engine.getRoot();
        is_gui = engine.getGui();
        loadParameters(world.getObjectField("application"));
        if (is_gui){
            std::cout << world.stringify() << std::endl;
            std::cerr << "Model parameters were sent to the output stream. Simulation aborted\n";
        }
    }
    broadcastParameterList();

}


void Application::loadParameterList(const param::Object &source) {
    std::cerr << "Loading application parameters...";
    mpi::Communicator& comm = getAppCommunicator();
    parent = source.getStringField("parent");
    process_number = source.getIntegerField("process_number");
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
    if (process_number != comm.getProcessorNumber() || is_gui) return;
    setOutputFolder(source.getStringField("output_folder_prefix"));
    std::cerr << "\033[32;1mOK\033[0m\n";
}

void Application::setOutputFolder(const std::string &folder_prefix) {
    output_folder = folder_prefix;
    for (auto info: sys::Folder(".")){
        std::cout << info.d_name << std::endl;
    }
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
    // std::cout << "CONTINUE BROADCASTING PARAMETERS\n";
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
