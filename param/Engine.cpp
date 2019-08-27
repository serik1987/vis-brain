//
// Created by serik1987 on 26.08.19.
//

#include <iostream>
#include "Engine.h"
#include "exceptions.h"
#include "../sys/FileInfo.h"

using namespace param;
using namespace v8;

Engine* Engine::instance = NULL;

Engine::Engine(int* argc, char*** argv){
    if (instance != NULL){
        throw DuplicateEngineException();
    }
    instance = this;
    V8::InitializeICUDefaultLocation(**argv);
    V8::InitializeExternalStartupData(**argv);
    platform = platform::NewDefaultPlatform();
    V8::InitializePlatform(platform.get());
    V8::Initialize();
    createParams.array_buffer_allocator = ArrayBuffer::Allocator::NewDefaultAllocator();
    isolate = Isolate::New(createParams);
    std::cout << "V8 engine was loaded\n";
    std::cout << "Zero parameter: " << **argv << std::endl;
    loadAll();
}

Engine::~Engine(){
    std::cout << "V8 engine was quited\n";
    isolate->Dispose();
    V8::Dispose();
    V8::ShutdownPlatform();
    delete createParams.array_buffer_allocator;
}

void Engine::loadAll(){
    std::cout << "Loading all scripts. Started...\n";
    HandleScope handle_scope(isolate);
    Local<Context> context = Context::New(isolate);
    Context::Scope context_scope(context);
    executeFile(context, std::string(APP_FOLDER) + std::string(STANDARD_JS_OPTIONS));
    // executeFile(context, "test.js");
    std::cout << "Loading all scripts. Finished...\n";
    std::cout << "Trying script method:\n";
    Local<Value> world_raw = executeCode(context, "(reading world object)", "world");
    if (!world_raw->IsObject()){
        throw NoObjectError();
    }
    auto world = world_raw->ToObject(context).ToLocalChecked();
    auto application_raw = world->Get(context,
            String::NewFromUtf8(isolate, "application").ToLocalChecked()).ToLocalChecked();
    std::cout << "Application info: " << application_raw->IsObject() << std::endl;
    auto application = application_raw->ToObject(context).ToLocalChecked();
    auto process_number_raw = application->Get(context, String::NewFromUtf8(isolate, "process_number").ToLocalChecked())
            .ToLocalChecked();
    auto process_number = process_number_raw->ToInteger(context).ToLocalChecked();
    auto final_process_number = process_number->Int32Value(context).ToChecked();
    std::cout << "Total number of processes: " << final_process_number << std::endl;
}

void Engine::executeFile(Local<Context>& context, std::string filename){
    std::cout << "Executing " << filename << std::endl;
    sys::FileInfo info(filename.c_str());
    off_t size = info.getFileSize();
    std::fstream file = info.openStream(std::fstream::in);
    char* buffer = new char[size+1];
    file.read(buffer, size);
    file.close();
    executeCode(context, filename, buffer);
    delete [] buffer;
}

Local<Value> Engine::executeCode(Local<Context>& context, std::string filename, const char* code){
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
    return handle_scope.Escape(final_result);
}