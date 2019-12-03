//
// Created by serik1987 on 03.12.2019.
//

#include <regex>
#include <iostream>
#include "security.h"

namespace sys{

    void security_check(const std::string& name, const std::string& value){
        static const std::regex SECURITY_CHECK_EXPRESSION(R"([\w\d\-]*)");
        if (!std::regex_match(value, SECURITY_CHECK_EXPRESSION)){
            throw security_check_error(name);
        }
    }

}