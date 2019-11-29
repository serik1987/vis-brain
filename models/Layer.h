//
// Created by serik1987 on 27.11.2019.
//

#ifndef MPI2_LAYER_H
#define MPI2_LAYER_H

#include "AbstractNetwork.h"
#include "../data/Matrix.h"
#include "../methods/Method.h"

namespace net {

    /**
     * Base class for all models where one layer only is represented
     */
    class Layer: public AbstractNetwork {
    public:
        /**
         * Returns the layer status within the current process
         *
         * Active - the layer shall be simulated and all its processors shall be included into the state.
         * Ghosted - the layer shall not be simulated but it shall be communicated via the graph communications
         * with another layer created by another process
         * Disabled - the layer shall be totally ignored during the simulation
         */
        enum Status {Active, Ghosted, Disabled};

    private:
        bool stimulus_acceptable = false;
        mpi::Communicator* comm = nullptr;
        Status status;
        method::Method* method = nullptr;
        bool scanned = false;

    protected:
        [[nodiscard]] const char* getObjectType() const noexcept override { return "layer"; }
        void loadParameterList(const param::Object& source) override;
        void broadcastParameterList() override;
        void setParameter(const std::string& name, const void* pvalue) override;
        void getNetworkConfiguration(std::ostream& out, int level) override;

        /**
         *
         * @return short layer description
         */
        virtual std::string getLayerDescription() = 0;

        /**
         * Loads all layer parameters except: type, mechanism, stimulus_acceptable
         *
         * @param source parameter source (a cover for v8::Object)
         */
        virtual void loadLayerParameters(const param::Object& source) = 0;

        /**
         * Broadcasts all layer parameters
         */
        virtual void broadcastLayerParameters() = 0;

        /**
         * Adds the layer processors to the state suggesting that there is no output processors connecting
         * to this certain processors, all processors within the layer as well as between this layer and all connected
         * layers are interconnected to each other
         *
         * @param state reference to the state
         */
        virtual void immediateAddToState(equ::State& state) = 0;

        /**
         * Returns the layer input data in case when the layer is not stimulus-acceptable
         * When the layer is stimulus-acceptable, its behavior is not predictable.
         *
         * @return pointer to the processor
         */
        virtual equ::Processor* getLayerInputData() = 0;

    public:
        explicit Layer(const std::string& name, const std::string& parent): AbstractNetwork(name, parent),
            scanned(false) {};

        static Layer* createLayer(const std::string& name, const std::string& parent_name,
                const std::string& mechanism);

        /**
         *
         * @return true if the layer accepts visual stimulus
         */
        [[nodiscard]] bool getStimulusAcceptable() const { return stimulus_acceptable; }

        /**
         * Sets whether the layer accepts visual stimuli
         *
         * @param value true if this accepts the stimuli, false otherwise
         */
        void setStimulusAcceptable(bool value) { stimulus_acceptable = value; }

        /**
         *
         * @return the layer communicator. This communicator will be applied to all layer processor and all layer
         * matrices
         */
        [[nodiscard]] mpi::Communicator& getCommunicator() { return *comm; }

        /**
         * Sets the layer communicator. This communicator will be applied to all processors and all matrices within
         * the layer
         *
         * @param c reference to the communicator
         */
        void setCommunicator(mpi::Communicator& c) { comm = &c; }

        /**
         *
         * @return current status of the layer
         */
        [[nodiscard]] Status getStatus() const { return status; }

        /**
         * Sets the new status
         *
         * @param value new status
         */
        void setStatus(Status value) { status = value; }

        /**
         *
         * @return current method which will be used for the integration
         */
        [[nodiscard]] method::Method& getMethod() { return *method; }

        /**
         * Sets the integration method for the network
         *
         * @param value reference to the method
         */
        void setMethod(method::Method& value) { method = &value; }

        /**
         *
         * @return true if the layer has been scanned for the purpose of adding all processors to the state
         */
        bool isScanned() { return scanned; }

        /**
         *
         * @param value
         */
        void setScanned(bool value) override { scanned = value; }

        void addProcessorsToState(equ::State& state) override;

        /**
         * Returns the processor that accepts input data to the layer or nullptr if the layer is
         * stimulus-acceptable
         *
         * @return input data to the processor
         */
        virtual equ::Processor* getInputData(){
            equ::Processor* proc = nullptr;

            if (getStimulusAcceptable()){
                proc = getLayerInputData();
            }

            return proc;
        }

        /**
         *
         * @return the processor that contains output data from the layer
         */
        virtual equ::Processor* getOutputData() = 0;
    };

}


#endif //MPI2_LAYER_H
