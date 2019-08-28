//
// Created by serik1987 on 26.08.19.
//

#include <iostream>
#include <cstring>
#include "Engine.h"
#include "exceptions.h"
#include "../sys/FileInfo.h"
#include "../sys/auxiliary.h"

using namespace param;
using namespace v8;

Engine* Engine::instance = NULL;

Engine::Engine(int* argc, char*** argv){
    if (instance != NULL){
        throw DuplicateEngineException();
    }
    parseParameters(*argc, *argv);
    if (help){
        getHelp();
        exit(0);
    }
    instance = this;
    V8::InitializeICUDefaultLocation(**argv);
    V8::InitializeExternalStartupData(**argv);
    platform = platform::NewDefaultPlatform();
    V8::InitializePlatform(platform.get());
    V8::Initialize();
    createParams.array_buffer_allocator = ArrayBuffer::Allocator::NewDefaultAllocator();
    isolate = Isolate::New(createParams);
    Isolate::Scope isolate_scope(isolate);
    HandleScope handle_scope(isolate);
    auto global = ObjectTemplate::New(isolate);
    auto context = Context::New(isolate, NULL, global);
    Context::Scope context_scope(context);
    persistent_context = new Persistent<Context>(isolate, context);
    std::cout << "V8 engine was loaded\n";
    loadAll();
}

void Engine::getHelp() {
    std::cout << "Usage:\n"
                 "\t\033[1m--model filename\033[0m load model parameters placed in the given file\n"
                 "\t\033[1m--set js_code\033[0m load model parameters using the given code\n"
                 "\t\033[1m--update json_object\033[0m Use the parameter given in the object instead of given in the code\n"
                 "\t\033[1m--gui\033[0m Print all parameters as JSON object instead of simulating the network\n";
}

Engine::~Engine(){
    if (persistent_context != nullptr){
        delete persistent_context;
    }
    if (root != nullptr){
        delete root;
    }
    std::cout << "V8 engine was quited\n";
    isolate->Dispose();
    V8::Dispose();
    V8::ShutdownPlatform();
    delete createParams.array_buffer_allocator;
}

void Engine::loadAll(){
    HandleScope handle_scope(isolate);
    Local<Context> context(isolate->GetCurrentContext());
    Context::Scope context_scope(context);
    executeFile(context, std::string(APP_FOLDER) + std::string(STANDARD_JS_OPTIONS), false);
    executeFile(context, std::string(USER_FOLDER) + std::string(STANDARD_JS_OPTIONS), true);
    executeFile(context, std::string(STANDARD_PROJECT_OPTIONS), true);
    executeFile(context, auxiliary_script, true);
    executeCode(context, "", auxiliary_code.c_str());
    if (!auxiliary_data.empty()) {
        executeCode(context, "", ("world = " + auxiliary_data).c_str());
    }
    Local<Value> world_raw = executeCode(context, "(reading world object)", "world");
    if (!world_raw->IsObject()){
        throw NoObjectError();
    }
    auto world = world_raw->ToObject(context).ToLocalChecked();
    root = new param::Object(isolate, "world", world);
}

void Engine::executeFile(Local<Context>& context, std::string filename, bool create_if_not_exists){
    if (filename == "") return;
    auto pos = filename.find("~");
    if (pos != std::string::npos){
        filename = filename.replace(0, 1, sys::get_home_directory());
    }
    std::cout << "Executing " << filename << "...";
    // try {
        off_t size;
        try {
            size = sys::FileInfo(filename.c_str()).getFileSize();
        } catch (sys::file_not_exists &e) {
            if (!create_if_not_exists) throw e;
            sys::create_empty_file(filename);
            size = sys::FileInfo(filename.c_str()).getFileSize();
        }
        if (size != 0) {
            std::fstream file(filename);
            char *buffer = new char[size + 1];
            file.read(buffer, size);
            file.close();
            executeCode(context, filename, buffer);
            delete[] buffer;
        }
        std::cout << "\033[32m\033[1mOK\033[0m\n";
        /*
    } catch (std::exception& e){

    }
         */

}

Local<Value> Engine::executeCode(Local<Context>& context, std::string filename, const char* code){
    if (filename.empty() && strlen(code) != 0){
        std::cout << "Executing {" << code << "}...";
    }
    EscapableHandleScope handle_scope(isolate);
    TryCatch try_catch(isolate);
    Local<String> source_obj = String::NewFromUtf8(isolate, code).ToLocalChecked();
    MaybeLocal<Script> script_obj = Script::Compile(context, source_obj);
    if (try_catch.HasCaught()){
        throw CompilationError(isolate, filename, try_catch);
    }
    Local<Script> script = script_obj.ToLocalChecked();
    MaybeLocal<Value> result = script->Run(context);
    if (try_catch.HasCaught()){
        throw ExecutionError(isolate, filename, try_catch);
    }
    Local<Value> final_result = result.ToLocalChecked();
    if (filename.empty() && strlen(code) != 0){
        std::cout << "\033[32m\033[1mOK\033[0m\n";
    }
    return handle_scope.Escape(final_result);
}

void Engine::parseParameters(int argc, char **argv) {
    enum {mode_default, mode_model, mode_set, mode_update} mode = mode_default;
    for (int i=1; i < argc; i++){
        if (strncmp(argv[i], "--", 2) == 0){
            char* mode_name = argv[i]+2;
            if (strcmp(mode_name, "model") == 0){
                mode = mode_model;
            } else if (strcmp(mode_name, "set") == 0){
                mode = mode_set;
            } else if (strcmp(mode_name, "update") == 0){
                mode = mode_update;
            } else if (strcmp(mode_name, "help") == 0){
                help = true;
                mode = mode_default;
            } else if (strcmp(mode_name, "gui") == 0){
                gui = true;
                mode = mode_default;
            } else {
                throw ModeError(mode_name);
            }
        } else {
            switch (mode){
                case mode_model:
                    auxiliary_script += argv[i];
                    break;
                case mode_set:
                    auxiliary_code += argv[i];
                    break;
                case mode_update:
                    auxiliary_data += argv[i];
                    break;
                default:
                    throw ModeArgumentError(argv[i]);
            }
        }
    }
}