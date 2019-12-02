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

#ifndef MPI2_APPLICATION_H
#define MPI2_APPLICATION_H

#include <exception>
#include "mpi/App.h"
#include "param/Loadable.h"
#include "param/Object.h"
#include "log/Engine.h"
#include "param/Engine.h"
#include "data/noise/NoiseEngine.h"
#include "exceptions.h"
#include "stimuli/Stimulus.h"
#include "methods/Method.h"
#include "models/Brain.h"
#include "jobs/Job.h"
#include "methods/Distributor.h"
#include "processors/State.h"


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
     * Loads all application parameters in the application presented in your JS code.
     * This also loads V8 engine and parses all JS code
     * Application parameters are contained in world.application JS object
     * If --help option is given the application returns the help string and terminates
     * If --gui option is given the application prints all parameters as JSON string into standard output stream
     *
     * @throws exception when there is an error in the JS code
     */
    void loadAllParameters();

    /**
     * Clears logging engine and V8 engine
     */
    void clearEngines();


    /**
     * Starts the simulation process
     */
    void simulate();

    /**
     * Clears V8 engine
     */
    void deleteV8Engine();

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
     * Indicates whether application shall be re-started. The value shall be checked after loadAllParameter() function
     * will be applied
     *
     * @return empty string if the application shall continue working.
     * Non-empty string if application is needed to be restarted by running a command given in the return results
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

    /**
     * Returns the parameter engine
     * ATTENTION! PLEASE, DON'T RUN THIS METHOD ON PROCESSES WITH RANK DIFFERENT FROM ZERO
     *
     * @return
     */
    param::Engine& getParameterEngine(){
        if (getAppCommunicator().getRank() != 0){
            std::cerr << "Application::getParameterEngine has been run by the process with rank different "
                         "from zero. The application shall hang for this. Please, check this error and don't do it "
                         "in the future\n";
            throw ParamEngineNotReady();
        }
        return *engine;
    }


    /**
     *
     * @return current logging engine
     * @throws logging_engine_is_not_ready when the logging engine has not been created yet
     */
    logging::Engine& getLoggingEngine(){
        if (log == nullptr){
            throw logging_engine_is_not_ready();
        }
        return *log;
    }

    /**
     *
     * @return reference to the noise engine
     */
    data::noise::NoiseEngine& getNoiseEngine(){
        if (gen == nullptr){
            log->progress(0, 1, "Initializing PRNG");
            gen = new data::noise::NoiseEngine();
        }
        return *gen;
    }

    void setParameter(const std::string& name, const void* pvalue) override;

    /**
     * Reads the world.stimulus parameter and creates a new stimulus
     *
     * @param comm communicator for which the stimulus shall be created, certain communicator value shall be
     * returned after the job has been created
     */
    void createStimulus(mpi::Communicator& comm);

    /**
     * Creates new integration method
     */
    void createMethod();

    /**
     * Creates new neural network to simulate and loads all its parameters
     */
    void createBrain();

    /**
     * Loads the main job parameters
     */
    job::Job*  getMainJob();

    /**
     *
     * @return pointer to the stimulus
     */
    stim::Stimulus& getStimulus(){
        return *stimulus;
    }

    /**
     * Retuns the general neural network that will be simulated
     *
     * @return the brain
     */
    net::Brain& getBrain(){
        return *brain;
    }

    /**
     *
     * @return reference to the current state
     */
    equ::State& getState() { return *state; }

    /**
     *
     * @return the integration method
     */
    method::Method& getMethod() { return *method; }

    /**
     *
     * @return returns the distributor that has been recently created
     */
    method::Distributor& getDistributor() { return *distributor; }

    /**
     * Creates new distributor and applies the distributor to the current brain
     *
     * @param comm reference to the communicator that will be used for creating the distributor
     * @param method reference to the selected integration method
     * @param apply if true the distributor will be applied to the current brain after creating
     * if false, the distributor shall be applied manually
     */
    void createDistributor(mpi::Communicator& comm, method::Method& method, bool apply = true);

    /**
     * Creates new state
     *
     * @param comm communicator that will be based on the state create
     */
    void createState(mpi::Communicator& comm);

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


    param::Engine* engine = nullptr;
    logging::Engine* log = nullptr;
    data::noise::NoiseEngine* gen = nullptr;
    stim::Stimulus* stimulus = nullptr;
    method::Method* method = nullptr;
    net::Brain* brain = nullptr;
    method::Distributor* distributor = nullptr;
    equ::State* state = nullptr;


    void setOutputFolder(const std::string& folder_prefix);
    std::string getDate();

};


#endif //MPI2_APPLICATION_H
