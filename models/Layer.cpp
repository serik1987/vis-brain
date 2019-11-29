//
// Created by serik1987 on 27.11.2019.
//

#include "Layer.h"
#include "abstract/AbstractModel.h"
#include "../log/output.h"

namespace net{

    void Layer::loadParameterList(const param::Object &source) {
        logging::progress(0, 1, "Loading parameters of '" + getFullName() + "'");
        if (getDeepFlag()){
            logging::info("Parameter of the layer '" + getFullName() + "'");
        }
        setStimulusAcceptable(source.getBooleanField("stimulus_acceptable"));
        if (getDeepFlag()){
            if (getStimulusAcceptable()){
                logging::info("The layer is stimulus-acceptable");
            } else {
                logging::info("The layer is not stimulus acceptable");
            }
        }
        loadLayerParameters(source);
    }

    void Layer::broadcastParameterList() {
        logging::progress(0, 1, "Broadcasting '" + getFullName() + "'");
        Application::getInstance().broadcastBoolean(stimulus_acceptable, 0);
        broadcastLayerParameters();
    }

    void Layer::setParameter(const std::string &name, const void *pvalue) {
        logging::enter();
        logging::debug("SET PARAMETER");
        logging::exit();
    }

    void Layer::getNetworkConfiguration(std::ostream &out, int level) {
        for (int i=0; i < level; ++i){
            out << "\t";
        }
        out << getName() << ": " << getLayerDescription() << "\n";
    }

    Layer *Layer::createLayer(const std::string &name, const std::string &parent_name, const std::string &mechanism) {
        using std::string;
        Layer* layer = nullptr;

        auto separator = mechanism.find(':');
        if (separator == string::npos){
            throw param::UnknownMechanism(mechanism);
        }
        auto mechanism_class = mechanism.substr(0, separator);
        auto mechanism_name = mechanism.substr(separator+1);

        if (mechanism_class == "abstract"){
            layer = AbstractModel::createAbstractModel(name, parent_name, mechanism_name);
        } else {
            throw param::UnknownMechanism(mechanism);
        }

        return layer;
    }

    void Layer::addProcessorsToState(equ::State &state) {
        if (!isScanned()) {
            Layer *output_layer = this;

            // Now we need to look for all output layers. However, because network connections are not still implemented,
            // let's assume that this layer is an output layer
            output_layer->immediateAddToState(state);
            output_layer->setScanned(true);
        }
    }
}