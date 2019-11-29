//
// Created by serik1987 on 27.11.2019.
//

#include "Network.h"
#include "../log/output.h"
#include "Layer.h"

namespace net{

    Network::~Network() {
        for (auto& pair: content){
            delete pair.second;
            pair.second = nullptr;
        }
    }

    void Network::loadParameterList(const param::Object &source) {
        if (!getDeepFlag()) {
            logging::progress(0, 1, "Loading parameters of the network '" + getName() + "'");
        }
        param::Object content_parameters = source.getObjectField("content");
        for (auto name: content_parameters){
            auto value = content_parameters.getObjectField(name);
            AbstractNetwork *net;
            if (getDeepFlag()) {
                net = content.at(name);
            } else {
                if (content.find(name) != content.end()) {
                    throw duplicate_network(name, getFullName());
                }
                std::string type = value.getStringField("type");
                if (type == "network") {
                    net = new Network(name, getFullName());
                } else if (type == "layer") {
                    std::string mechanism = value.getStringField("mechanism");
                    layerMechanisms.push_back(mechanism);
                    net = Layer::createLayer(name, getFullName(), mechanism);
                } else {
                    throw param::WrongObjectType();
                }
                content[name] = net;
            }
            if (net != nullptr) {
                net->loadParameters(value);
            }
        }
    }

    void Network::broadcastParameterList() {
        if (!getDeepFlag()) {
            broadcastChildNetworks();
        }
        for (auto item: content){
            item.second->broadcastParameters();
        }
    }

    void Network::broadcastChildNetworks() {
        auto& app = Application::getInstance();
        auto& comm = Application::getInstance().getAppCommunicator();
        logging::progress(0, 1, "Broadcasting the network '" + getName() + "'");
        if (comm.getRank() == 0){
            int data = 0;
            auto mainIterator = content.begin();
            auto mechanismIterator = layerMechanisms.begin();
            for (; mainIterator != content.end(); ++mainIterator){
                std::string key = mainIterator->first;
                AbstractNetwork* value = mainIterator->second;
                auto* network = dynamic_cast<Network*>(value);
                auto* layer = dynamic_cast<Layer*>(value);
                if (network != nullptr){
                    data = LAYER_CREATE_COMMAND_CREATE_NETWORK;
                }
                if (layer != nullptr){
                    data = LAYER_CREATE_COMMAND_CREATE_LAYER;
                }
                comm.broadcast(&data, 1, MPI_INT, 0);
                app.broadcastString(key, 0);
                if (layer != nullptr){
                    app.broadcastString(*mechanismIterator, 0);
                    ++mechanismIterator;
                }
            }
            data = LAYER_CREATE_COMMAND_STOP;
            comm.broadcast(&data, 1, MPI_INT, 0);
        } else {
            int data;
            while (true){
                bool finish;
                comm.broadcast(&data, 1, MPI_INT, 0);
                finish = (data == LAYER_CREATE_COMMAND_STOP);
                if (finish) break;
                std::string name;
                app.broadcastString(name, 0);
                AbstractNetwork* net;
                if (data == LAYER_CREATE_COMMAND_CREATE_NETWORK){
                    net = new Network(name, getFullName());
                }
                if (data == LAYER_CREATE_COMMAND_CREATE_LAYER){
                    std::string mechanism;
                    app.broadcastString(mechanism, 0);
                    net = Layer::createLayer(name, getFullName(), mechanism);
                }
                content[name] = net;
            }
        }
    }

    void Network::setParameter(const std::string &name, const void *pvalue) {
        logging::enter();
        logging::debug("SET PARAMETER LIST");
        logging::exit();
    }

    void Network::getNetworkConfiguration(std::ostream &out, int level) {
        for (int i=0; i < level; ++i){
            out << "\t";
        }
        out << getName() << "\n";
        for (auto pair: content){
            if (pair.second == nullptr){
                for (int i=0; i < level+1; ++i){
                    out << "\t";
                }
                out << "NETWORK CORRUPTED\n";
            } else {
                pair.second->getNetworkConfiguration(out, level + 1);
            }
        }
    }

    void Network::addProcessorsToState(equ::State &state) {
        for (auto pair: content){
            pair.second->addProcessorsToState(state);
        }
    }

    void Network::setScanned(bool value) {
        for (auto pair: content){
            pair.second->setScanned(value);
        }
    }

    Layer *Network::getLayerByFullName(const std::string &fullName) {
        using std::string;
        Layer* layer;

        auto separator = fullName.find('.');
        string name;
        string subName;
        if (separator == string::npos){
            AbstractNetwork* net;
            try{
                net = content.at(fullName);
            } catch (std::out_of_range& e){ // these lines decline the error status
                // from 'fatal exception' which results to the program termination to
                // the 'simulation exception' which results to the termination of the current
                // job and moving to the next job
                throw layer_not_found(getFullName(), fullName);
            }
            layer = dynamic_cast<Layer*>(net);
            if (layer == nullptr){
                throw layer_not_found(getFullName(), fullName);
            }
        } else {
            AbstractNetwork* net;
            try{
                net = content.at(fullName);
            } catch (std::out_of_range& e){
                throw layer_not_found(getFullName(), fullName);
            }
            auto* container = dynamic_cast<Network*>(net);
            if (container == nullptr){
                throw layer_not_found(getFullName(), fullName);
            }
            layer = container->getLayerByFullName(subName);
        }

        return layer;
    }
}