//
// Created by serik1987 on 30.11.2019.
//

#include "DistributorBuilder.h"
#include "EqualDistributor.h"
#include "../log/output.h"

namespace method {

    void DistributorBuilder::loadParameterList(const param::Object &source) {
        logging::progress(0, 1, "Application of the network distributor");
        distributor_name = source.getStringField("distributor");
        logging::info("");
        logging::info("Distributor information:");
        logging::info("Distributor type: " + distributor_name);
    }

    void DistributorBuilder::broadcastParameterList() {
        Application::getInstance().broadcastString(distributor_name, 0);
    }

    Distributor *DistributorBuilder::build() {
        Distributor* dist = nullptr;

        if (distributor_name == "equal"){
            dist = new EqualDistributor(comm, method);
        } else {
            throw incorrect_distributor();
        }

        return dist;
    }
}