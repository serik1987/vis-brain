//
// Created by serik1987 on 26.08.19.
//

#ifndef MPI2_ENGINE_H
#define MPI2_ENGINE_H

#include <v8.h>
#include <libplatform/libplatform.h>
#include "../compile_options.h"

namespace param {


    /**
     * Represents the parameter engine
     *
     * The parameter engine enters and exits the V8 engine and provides an access to all JS objects
     */
    class Engine {
    public:

        /**
         * Initialization: no parameters
         */
        Engine(int* argc, char*** argv);
        Engine(const Engine&) = delete;

        /**
         * Destruction
         */
        ~Engine();



    private:
        static Engine* instance;
        std::unique_ptr<v8::Platform> platform;
        v8::Isolate::CreateParams createParams;
        v8::Isolate* isolate;

        void loadAll();
        void executeFile(v8::Local<v8::Context>& context, std::string filename);
        v8::Local<v8::Value> executeCode(v8::Local<v8::Context>& context, std::string filename, const char* code);

    };

}


#endif //MPI2_ENGINE_H
