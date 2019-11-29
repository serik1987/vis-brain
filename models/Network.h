//
// Created by serik1987 on 27.11.2019.
//

#ifndef MPI2_NETWORK_H
#define MPI2_NETWORK_H

#include <unordered_map>
#include "AbstractNetwork.h"
#include "../processors/State.h"
#include "Layer.h"

namespace net {

    /**
     * A base class for all models that consist of more elementary child models and connections between them
     */
    class Network: public AbstractNetwork {
    private:
        std::unordered_map<std::string, AbstractNetwork*> content;
        std::list<std::string> layerMechanisms;

        static constexpr int LAYER_CREATE_COMMAND_STOP = 0x00;
        static constexpr int LAYER_CREATE_COMMAND_CREATE_NETWORK = 0x01;
        static constexpr int LAYER_CREATE_COMMAND_CREATE_LAYER = 0x02;

        void broadcastChildNetworks();

    protected:
        [[nodiscard]] const char* getObjectType() const noexcept override { return "network"; }
        void loadParameterList(const param::Object& source) override;
        void broadcastParameterList() override;
        void setParameter(const std::string& name, const void* pvalue) override;
        void getNetworkConfiguration(std::ostream& out, int level) override;

    public:
        explicit Network(const std::string& name, const std::string& parent = ""): AbstractNetwork(name, parent) {};

        ~Network() override;

        class duplicate_network: public std::exception{
        private:
            std::string message;

        public:
            duplicate_network(const std::string& network_name, const std::string& parent_name){
                std::stringstream ss;
                ss << "Network '" << network_name << "' is duplicated within the parent network '" + parent_name + "'";
                message = ss.str();
            }

            [[nodiscard]] const char* what() const noexcept override {
                return message.c_str();
            }
        };

        class layer_not_found: public simulation_exception{
        private:
            std::string message;

        public:
            layer_not_found(const std::string network_name, const std::string layer_name){
                std::stringstream ss;
                ss << "Layer '" << layer_name << "' is not a valid layer within the network '" << network_name << "'";
                message = ss.str();
            }

            [[nodiscard]] const char* what() const noexcept override{
                return message.c_str();
            }
        };

        /**
         *
         * @return reference to the map of all children elements
         */
        [[nodiscard]] std::unordered_map<std::string, AbstractNetwork*>& getChildNetworks() { return content; }

        /**
         * Adds all processors belonging to the child networks and layers to the state
         *
         * @param state reference to the state
         */
        void addProcessorsToState(equ::State& state) override;

        void setScanned(bool value) override;

        /**
         * Access the layer by its full name
         * Layer full names are names of all child networks mentioned through the dot (.)
         *
         * @param fullName names of all children
         * @return pointer to the layer
         */
        Layer* getLayerByFullName(const std::string& fullName);
    };

}


#endif //MPI2_NETWORK_H
