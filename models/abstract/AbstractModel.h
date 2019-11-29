//
// Created by serik1987 on 28.11.2019.
//

#ifndef MPI2_ABSTRACTMODEL_H
#define MPI2_ABSTRACTMODEL_H

#include "../Layer.h"

namespace net {

    /**
     * A base class for all abstract mathematical models. These models are formulated in terms of
     * single layer models
     */
    class AbstractModel: public Layer {
    protected:
        void loadLayerParameters(const param::Object& source) override;
        void broadcastLayerParameters() override;

        /**
         * Loads all parameters of the abstract models except type, mechanism, stimulus_acceptable;
         * without establishing connections between the processes
         *
         * @param source parameter source
         */
        virtual void loadAbstractModelParameters(const param::Object& source) = 0;

        /**
         * Broadcasts all parameters of the abstract models except type, mechanism, stimulus_acceptable;
         * without establishing connections between the processes
         */
        virtual void broadcastAbstractModelParameters() = 0;

    public:
        explicit AbstractModel(const std::string& name, const std::string& parent = ""): Layer(name, parent) {};

        static AbstractModel* createAbstractModel(const std::string& name, const std::string& parent,
                const std::string& mechanism);
    };

}


#endif //MPI2_ABSTRACTMODEL_H
