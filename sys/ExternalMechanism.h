//
// Created by serik1987 on 14.11.2019.
//

#ifndef MPI2_EXTERNALMECHANISM_H
#define MPI2_EXTERNALMECHANISM_H

#include <string>
#include <dlfcn.h>
#include "exceptions.h"
#include "../log/exceptions.h"

namespace sys {

    /**
     * A base class for all mechanisms which implementation is separated from the main vis-brain executable file
     * (They are represented as plugins)
     */
    class ExternalMechanism {
    private:
        void* libraryHandle;
        int (*get_parameter_list)(int* pname, const char*** plist);
        int (*get_parameter_value)(const char* name, double* pvalue);
        int (*set_parameter_value)(const char* name, double value);
        int (*initialize_buffers)();
        int (*finalize_buffers)();
        int parameter_number;
        const char** parameter_names;

    protected:
        /**
         * By default, external mechanism provides loading of such functions as:
         * get_parameter_list, get_parameter_value, set_parameter_value, initialize_buffers, finalize_buffers
         * In order to load the other functions use this library
         *
         * @param name name of a function to load
         * @return pointer to this function
         */
        void* loadFunction(const std::string& name);

        /**
         * Sets the external parameter
         *
         * @param name parameter name
         * @param value parameter value
         */
        void setExternalParameter(const char* name, double value){
            if (set_parameter_value(name, value)){
                throw parameter_not_exists();
            }
        }

        /**
         * Creates all buffers associated with an external mechanism. The method shall be started during execution
         * of the processor's method initialize()
         */
        void initializeExternalMechanism(){
            initialize_buffers();
        }

        /**
         * Destroys all buffers associated with an external mechanism. The method shall be started during execution
         * of the processor's method finalize()
         */
        void finalizeExternalMechanism(){
            finalize_buffers();
        }

    public:
        class mechanism_loading_error: public exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "Failed to load external mechanism";
            }
        };

        class function_loading_error: public exception{
        private:
            std::string message;

        public:
            explicit function_loading_error(const std::string& function_name){
                message = "Failure to load the following function from an external mechanism: " + function_name;
            }

            [[nodiscard]] const char* what() const noexcept override{
                return message.c_str();
            }
        };

        class get_parameters_name_list_error: public exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "Failed to retrieve total number of mechanisms";
            }
        };

        class parameter_not_exists: public simulation_exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "Attempt to access to non-existent stimulation parameter";
            }
        };

        /**
         * Creates an external mechanism
         *
         * @param filename absolute path to the mechanism .so file (use equ::Processor::lookExternalMechanism to
         * get this name
         */
        explicit ExternalMechanism(const std::string& filename);

        virtual ~ExternalMechanism();

        /**
         *
         * @return total number of all parameters
         */
        [[nodiscard]] int getParameterNumber() const { return parameter_number; }

        /**
         * Gets the parameter name
         *
         * @param index index of the parameter
         * @return its name as string constant
         */
        [[nodiscard]] const char* getParameterName(int index) const { return parameter_names[index]; }

        /**
         * Returns the external parameter
         * External parameters are those that are transmitted to the external library and applied by the library
         * machinery. These parameters don't have corresponding getters. All parameters that have their corresponding
         * headers can't be revealed by means of this function. Use these getters (like getContrast, getLuminance etc.)
         *
         * @param name parameter name
         * @return parameter value
         */
        [[nodiscard]] double getParameter(const char* name){
            double result;
            if (get_parameter_value(name, &result)){
                throw parameter_not_exists();
            }
            return result;
        }
    };

}


#endif //MPI2_EXTERNALMECHANISM_H
