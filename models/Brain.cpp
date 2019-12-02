//
// Created by serik1987 on 27.11.2019.
//

#include "Brain.h"
#include "../Application.h"

namespace net{

    void Brain::createProcessors() {
        auto& app = Application::getInstance();

        if (app.getAppCommunicator().getRank() == 0){
            auto brain_parameters = app.getParameterEngine().getRoot().getObjectField("brain");
            loadParameters(brain_parameters);
        }
        broadcastParameters();
    }
}