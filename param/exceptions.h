//
// Created by serik1987 on 26.08.19.
//

#ifndef PARAM_EXCEPTIONS_H
#define PARAM_EXCEPTIONS_H

#include <exception>
#include <v8.h>
#include "../log/exceptions.h"

namespace param{

class exception: public std::exception {};

class root_error: public exception{
public:
    const char* what() const noexcept override{
        return "ROOT PROCESS TERMINATED\n";
    }
};

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

class TypeError: public exception {};

class IntegerTypeError: public TypeError{
private:
    std::string message;

public:
    IntegerTypeError(const std::string& object_name, const std::string& field_name): TypeError(){
        message = "'" + field_name + "' is not a valid integer field of '" + object_name + "'";
    }

    const char* what() const noexcept override{
        return message.c_str();
    }
};

class FloatTypeError: public TypeError{
private:
    std::string message;

public:
    FloatTypeError(const std::string& object_name, const std::string& field_name): TypeError(){
        message = "'" + field_name + "' is not a valid float field of '" + object_name + "'";
    }

    const char* what() const noexcept override{
        return message.c_str();
    }
};

class StringTypeError: public TypeError{
private:
    std::string message;

public:
    StringTypeError(const std::string& object_name, const std::string& field_name): TypeError() {
        message = "'" + field_name + "' is not a valid string of '" + object_name + "'";
    }

    const char* what() const noexcept override{
        return message.c_str();
    }
};

class ObjectTypeError: public TypeError{
private:
    std::string message;

public:
    ObjectTypeError(const std::string& object_name, const std::string& field_name): TypeError() {
        message = "'" + field_name + "' is not a child object of '" + object_name + "'";
    }

    const char* what() const noexcept override{
        return message.c_str();
    }
};

class BooleanTypeError: public TypeError{
private:
    std::string message;

public:
    BooleanTypeError(const std::string& object_name, const std::string& field_name): TypeError(){
        message = "'" + field_name + "' is not a boolean value of '" + object_name + "'";
    }

    [[nodiscard]] const char* what() const noexcept override{
        return message.c_str();
    }
};

class WrongObjectType: public TypeError{
public:
    [[nodiscard]] const char* what() const noexcept override{
        return "Object type given in JS doesn't correspond to the object type required by the parameter";
    }
};

class WrongMechanism: public TypeError{
public:
    [[nodiscard]] const char* what() const noexcept override{
        return "Incorrect JS object is substituted as a child";
    }
};

class UnknownMechanism: public TypeError{
public:
    [[nodiscard]] const char* what() const noexcept override{
        return "Unknown or unsupported mechanism was put";
    }
};

class SetApplicationParameterError: public simulation_exception{
public:
    [[nodiscard]] const char* what() const noexcept override{
        return "Can't set application parameter through the job";
    }
};

class IncorrectParameterName: public simulation_exception{
private:
    std::string message;

public:
    IncorrectParameterName(const std::string& parameter_name, const std::string& object_name){
        message = object_name + " has no parameter '" + parameter_name + "'";
    }

    [[nodiscard]] const char* what() const noexcept override{
        return message.c_str();
    }
};





}

#endif //MPI2_EXCEPTIONS_H
