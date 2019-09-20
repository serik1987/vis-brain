//
// Created by serik1987 on 26.08.19.
//

#ifndef MPI2_PARAMENGINE_H
#define MPI2_PARAMENGINE_H

#include <v8.h>
#include <libplatform/libplatform.h>
#include "../compile_options.h"
#include "Object.h"

namespace param {


    /**
     * Represents the parameter engine
     *
     * The parameter engine enters and exits the V8 engine and provides an access to all JS objects
     */
    class Engine {
    public:

        /**
         * Initialization: handles to all command line parameters
         */
        Engine(int* argc, char*** argv);
        Engine(const Engine&) = delete;

        /**
         * Destruction
         */
        ~Engine();

        /**
         * Checks for GUI mode.
         * The GUI mode is defined by --gui key. In this mode the simulation is not started. However,
         * the application sends the simulation data to the output stream
         *
         * @return true for GUI mode
         */
        bool getGui() const { return gui; }

        /**
         *
         * @return the root object that corresponds to the World Javascript object
         */
        const param::Object& getRoot() const {
            return *root;
        }

        /**
         *
         * @return A single instance to this engine The method is valid only if the instance has not been destroyed
         */
        static Engine& getInstance(){
            return *instance;
        }

        /**
         *
         * @return the context where all scripts were executed (for developmental use only)
         */
        v8::Persistent<v8::Context>& getContext(){
            return *persistent_context;
        }



    private:
        static Engine* instance;
        std::unique_ptr<v8::Platform> platform;
        v8::Isolate::CreateParams createParams;
        v8::Isolate* isolate;

        std::string auxiliary_script;
        std::string auxiliary_code;
        std::string auxiliary_data;
        bool gui = false;
        bool help = false;

        param::Object* root = nullptr;
        v8::Persistent<v8::Context>* persistent_context;

        /**
         * Returns a concise help
         */
        void getHelp();

        /**
         * Executes all files needed for vis-brain. Runs automatically on engine creation
         */
        void loadAll();

        /**
         * Parses all input parameters
         *
         * @param argc
         * @param argv
         */
        void parseParameters(int argc, char* argv[]);

        /**
         * Executes a specified file
         *
         * @param context context where the file shall be executed
         * @param filename name of the executed file
         * @param create_if_not_exists true if create empty file and folder, false otherwise
         */
        void executeFile(v8::Local<v8::Context>& context, std::string filename, bool create_if_not_exists);

        /**
         * Executes an arbitrary JS code
         *
         * @param context context where the code shall be launched
         * @param filename filename where the code belongs to (arbitrary string, influences on messages only, may be empty)
         * @param code the JS code itself
         * @return result of the code execution
         */
        v8::Local<v8::Value> executeCode(v8::Local<v8::Context>& context, std::string filename, const char* code);

    };

}


#endif //MPI2_PARAMENGINE_H
