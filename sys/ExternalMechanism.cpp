//
// Created by serik1987 on 14.11.2019.
//

#include "ExternalMechanism.h"
#include "../log/output.h"

namespace sys{

    ExternalMechanism::ExternalMechanism(const std::string &filename) {
        libraryHandle = dlopen(filename.c_str(), RTLD_LAZY);
        if (libraryHandle == NULL){
            mechanism_loading_error();
        }
        get_parameter_list = (int (*)(int*, const char***))loadFunction("get_parameter_list");
        if (get_parameter_list(&parameter_number, &parameter_names)){
            throw get_parameters_name_list_error();
        }
        get_parameter_value = (int (*)(const char*, double*))loadFunction("get_parameter_value");
        set_parameter_value = (int (*)(const char*, double))loadFunction("set_parameter_value");
        initialize_buffers = (int (*)())loadFunction("initialize_buffers");
        finalize_buffers = (int (*)())loadFunction("finalize_buffers");
    }

    ExternalMechanism::~ExternalMechanism() {
        if (dlclose(libraryHandle)){
            std::cerr << "ERROR DURING THE LIBRARY CLOSE\n";
        }
    }

    void *ExternalMechanism::loadFunction(const std::string &name) {
        void* result;
        result = dlsym(libraryHandle, name.c_str());
        if (result == NULL){
            throw function_loading_error(name);
        }
        return result;
    }
}