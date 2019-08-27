//
// Created by serik1987 on 26.08.19.
//

#ifndef MPI2_EXCEPTIONS_H
#define MPI2_EXCEPTIONS_H

#include <exception>
#include <v8.h>

namespace param{

class exception: public std::exception {};

class DuplicateEngineException: public exception{
public:
    const char* what() const noexcept override{
        return "Attempt to load two parameter engines";
    }
};

class JsError: public exception{
private:
    std::string filename;
    std::string error_message;
    std::string full_message;

protected:
    virtual const char* getProcessType() const =0;

    void getFullMessage(){
        full_message = std::string() + "Error in " + getProcessType() + " of " + filename + ": " + error_message;
    }

public:
    JsError(v8::Isolate* isolate, std::string fn, const v8::TryCatch& try_catch): filename(fn){
        v8::String::Utf8Value msg(isolate, try_catch.Message()->Get());
        error_message = std::string(*msg);
    }

    const char* what() const noexcept override{
        return full_message.c_str();
    }
};

class CompilationError: public JsError{
protected:
    const char* getProcessType() const{
        return "compilation";
    }

public:
    CompilationError(v8::Isolate* isolate, std::string filename, const v8::TryCatch& try_catch):
        JsError(isolate, filename, try_catch) {
        getFullMessage();
    };
};

class ExecutionError: public JsError{
protected:
    const char* getProcessType() const{
        return "execution";
    }

public:
    ExecutionError(v8::Isolate* isolate, std::string filename, const v8::TryCatch& try_catch):
        JsError(isolate, filename, try_catch) {
        getFullMessage();
    };
};

class NoObjectError: public exception{
public:
    const char* what() const noexcept override{
        return "all JS scripts has been run OK but don't define the world parameter as an object";
    }
};

class ModeError: public exception{
private:
    std::string message;
public:
    ModeError(const char* mode_name): exception(){
        message = std::string("") + "Incorrect argument given: --" + mode_name + "\n";
    }

    const char* what() const noexcept override{
        return message.c_str();
    }
};

class ModeArgumentError: public exception{
private:
    std::string message;
public:
    ModeArgumentError(const char* argument): exception(){
        message = std::string("")
                + "The following data can't be attributed to either argument or the argument parameter: " +
                argument;
    }

    const char* what() const noexcept override{
        return message.c_str();
    }
};

}

#endif //MPI2_EXCEPTIONS_H
