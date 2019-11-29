//
// Created by serik1987 on 28.11.2019.
//

#include "AbstractModel.h"
#include "glm/GlmLayer.h"

namespace net{

    AbstractModel *
    AbstractModel::createAbstractModel(const std::string& name, const std::string& parent,
            const std::string &mechanism) {
        using std::string;
        AbstractModel* model = nullptr;

        auto delimiter = mechanism.find('.');
        string major_name;
        string minor_name;
        if (delimiter == string::npos){
            major_name = mechanism;
            minor_name = "";
        } else {
            major_name = mechanism.substr(0, delimiter);
            minor_name = mechanism.substr(delimiter+1);
        }

        if (major_name == "glm"){
            model = new GlmLayer(name, parent);
        } else {
            throw param::UnknownMechanism("abstract:" + mechanism);
        }

        return model;
    }

    void AbstractModel::loadLayerParameters(const param::Object &source) {
        loadAbstractModelParameters(source);
    }

    void AbstractModel::broadcastLayerParameters() {
        broadcastAbstractModelParameters();
    }
}
