//
// Created by serik1987 on 30.11.2019.
//

#include "MethodBuilder.h"
#include "ExplicitEuler.h"
#include "ExplicitRecountEuler.h"
#include "ExplicitRungeKutta.h"
#include "KhoinMethod.h"
#include "../log/output.h"

namespace method{

    void MethodBuilder::loadParameterList(const param::Object &source) {
        logging::info("Information about the integration method");
        setIntegrationStep(source.getFloatField("integration_step"));
        logging::info("Integration step, ms: " + std::to_string(getIntegrationStep()));
        setIntegrationMethod(source.getStringField("integration_method"));
        logging::info("Integration method: " + getIntegrationMethod());
    }

    void MethodBuilder::broadcastParameterList() {
        auto& app = Application::getInstance();
        app.broadcastDouble(integration_step, 0);
        app.broadcastString(integration_method, 0);
    }

    Method *MethodBuilder::build() {
        using std::string;
        Method* method = nullptr;

        auto delimiter = getIntegrationMethod().find('.');
        string major_method_name;
        string minor_method_name;
        if (delimiter == string::npos){
            major_method_name = getIntegrationMethod();
        } else {
            major_method_name = getIntegrationMethod().substr(0, delimiter);
            minor_method_name = getIntegrationMethod().substr(delimiter+1);
        }

        if (major_method_name == "explicit-euler"){
            method = new ExplicitEuler(getIntegrationStep());
        } else if (major_method_name == "explicit-recount-euler"){
            method = new ExplicitRecountEuler(getIntegrationStep());
        } else if (major_method_name == "khoin"){
            method = new KhoinMethod(getIntegrationStep());
        } else if (major_method_name == "explicit-runge-kutta"){
            int n = stoi(minor_method_name);
            GET_EXPLICIT_RUNGE_KUTTA<1>(n, getIntegrationStep(), &method);
            GET_EXPLICIT_RUNGE_KUTTA<2>(n, getIntegrationStep(), &method);
            GET_EXPLICIT_RUNGE_KUTTA<3>(n, getIntegrationStep(), &method);
            /* Add the line above to add the following support of the Runge Kutta. Function argument shall be
             * the same, function parameters may be different*/
        }

        if (method == nullptr){
            throw incorrect_method();
        }

        return method;
    }
}