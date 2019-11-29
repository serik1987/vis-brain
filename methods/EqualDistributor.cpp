//
// Created by serik1987 on 28.11.2019.
//

#include "EqualDistributor.h"
#include "../models/Network.h"
#include "../models/Layer.h"

namespace method{

    void EqualDistributor::apply(net::AbstractNetwork &network) {
        auto* net = dynamic_cast<net::Network*>(&network);
        auto* layer = dynamic_cast<net::Layer*>(&network);

        if (net != nullptr){
            for (const auto& pair: net->getChildNetworks()){
                apply(*pair.second);
            }
        }

        if (layer != nullptr){
            layer->setCommunicator(getCommunicator());
            layer->setMethod(getMethod());
            layer->setStatus(net::Layer::Active);
        }

        network.setDeepFlag(true);
    }
}