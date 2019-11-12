//
// Created by serik1987 on 11.11.2019.
//

#include "Loadable.h"
#include "../Application.h"

namespace param{

    void Loadable::loadParameters(const param::Object &source){
        try {
            if (source.getStringField("type") != getObjectType()) {
                throw WrongObjectType();
            }
            loadParameterList(source);
        } catch (std::exception& e){
            int error_code = -1;
            Application::getInstance().broadcastInteger(error_code, 0);
            throw;
        }
    }

    void Loadable::broadcastParameters(){
        int error_code = 0;
        Application::getInstance().broadcastInteger(error_code, 0);
        if (error_code != 0){
            throw root_error();
        }
        broadcastParameterList();
    }

}