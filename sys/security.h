//
// Created by serik1987 on 03.12.2019.
//

#ifndef MPI2_SECURITY_H
#define MPI2_SECURITY_H

#include <string>
#include <sstream>
#include "../log/exceptions.h"

namespace sys{

    class security_check_error: public simulation_exception{
    private:
        std::string message;

    public:
        explicit security_check_error(const std::string& parameter_name){
            std::stringstream ss;
            ss << "Parameter " << parameter_name << " contains symbols that are not allowed because of security reasons";
            message = ss.str();
        }

        [[nodiscard]] const char* what() const noexcept override{
            return message.c_str();
        }
    };

    void security_check(const std::string& name, const std::string& value);

}


#endif //MPI2_SECURITY_H
