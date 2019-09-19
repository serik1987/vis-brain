//
// Created by serik1987 on 28.08.19.
//

#ifndef MPI2_APPLICATION_H
#define MPI2_APPLICATION_H

#include <exception>
#include "mpi/App.h"
#include "param/Loadable.h"
#include "param/Object.h"


/**
 * General class that represents all application
 */
class Application: public mpi::App, public param::Loadable {

protected:
    void loadParameterList(const param::Object& source) override;
    void broadcastParameterList() override;

public:

    /**
     * Simple Application will be run on stand-alone computer
     * External Application will be run on a set of computers
     */
    enum ConfigurationMode {Simple, External};

    /**
     * Initialization
     *
     * @param argc Pointer to total number of arguments
     * @param argv Pointer to the arguments array
     */
    Application(int* argc, char*** argv);

    ~Application();

    /**
     * Loads all parameters in the application presented in your JS code
     * If --help option is given the application returns the help string and terminates
     * If --gui option is given the application prints all parameters as JSON string into standard output stream
     *
     * @throws exception when there is an error in the JS code
     */
    void loadAllParameters();

    /**
     * A single application instance is created during execution of the main() function in main.cpp file
     * Any other Application instances shall not be created. If you need to access this instance, please
     * use this static method
     *
     * @return an instance to the application.
     */
    static Application& getInstance(){
        return *instance;
    }

    /**
     * Checks whether application is ready. Application is considered is ready when all conditions were met
     * 1. All JS parameters were loaded
     * 2. Number of MPI processes launched are the same as number of processes set in world.application.process_number
     * option
     * 3. Application doesn't run with --gui option
     *
     * @return true if application is ready
     */
    bool isApplicationReady(){
        return application_ready;
    }

    /**
     *
     * @return true if application run with --gui option
     */
    bool isGui(){
        return is_gui;
    }

    /**
     * If the number of MPI processes are not the same as the number
     *
     * @return
     */
    const std::string& getCmd() const{
        return cmd;
    }


    /**
     *
     * @return the directory where simulation data were located
     */
    const std::string& getOutputFolder() const{
        return output_folder;
    }

private:
    static Application* instance;
    bool application_ready = false;

    void fillCmd();

    const char* getObjectType() const noexcept override{
        return "application";
    }

    std::string parent;
    ConfigurationMode configuration_mode;
    int process_number;
    std::string host_configuration_file;
    bool is_gui;
    std::string cmd;
    int argument_number;
    char** argument_pointer;
    std::string output_folder;


    void setOutputFolder(const std::string& folder_prefix);
    std::string getDate();

};


#endif //MPI2_APPLICATION_H
