//
// Created by serik1987 on 27.11.2019.
//

#ifndef MPI2_ABSTRACTNETWORK_H
#define MPI2_ABSTRACTNETWORK_H

#include <sstream>
#include "../processors/Processor.h"
#include "../processors/State.h"
#include "../param/Loadable.h"

namespace net {

    /**
     *
     */
    class AbstractNetwork: public param::Loadable {
    private:
        std::string name;
        std::string fullname;
        bool deepFlag;

    public:

        /**
         *
         * @param name given name for the network
         * @param parent full name of the parent network
         */
        explicit AbstractNetwork(const std::string& name, const std::string& parent = ""): name(name) {
            if (parent.empty()){
                fullname = name;
            } else {
                fullname = parent + "." + name;
            }
            deepFlag = false;
        };

        ~AbstractNetwork() override {};

        /**
         *
         * @return name of the certain network
         */
        [[nodiscard]] virtual const std::string& getName() const { return name; }

        /**
         *
         * @return full name of the certain network
         */
        [[nodiscard]] virtual const std::string& getFullName() const { return fullname; }

        /**
         * Saves the network configuration to the output stream
         *
         * @param out output stream where the network configuration will be saved
         * @param level 0 everywhere except when called by the same method belonging to the parent network
         * (in this case the level shall be equal to the level value for the parent network plus 1)
         */
        virtual void getNetworkConfiguration(std::ostream& out, int level) = 0;

        /**
         * Writes full information about the network configuration
         *
         * @return string where the information shall be written
         */
        std::string printNetworkConfiguration(){
            std::stringstream ss;
            getNetworkConfiguration(ss, 0);
            return ss.str();
        }

        /**
         * Returns the deep flag. If the deep flag is true, loading/broadcasting network parameters will result
         * to creation of new processes and establishing connections between the processes.
         * If deep flag is false, loading/broadcasting betwork parameters will result to the
         * creation of new networks/layers and connecting layers to each other
         *
         * @return the deep flag status (true or false)
         */
        bool getDeepFlag() { return deepFlag; }

        /**
         * The method shall be applied by the distirbutor that shall set deep flag to true
         * After the deep flag was set, all network parameters shall be loaded/broadcasted again despite
         * of whether certain layer if active or inactive
         *
         * @param value
         */
        void setDeepFlag(bool value) { deepFlag = value; }

        /**
         * Adds all processors within the network to the processor state
         *
         * @param state reference to the state
         */
        virtual void addProcessorsToState(equ::State& state) = 0;

        /**
         * Changes the SCANNED flag for all layers within the network to a certain value
         *
         * @param value true or false
         */
        virtual void setScanned(bool value) = 0;
    };

}


#endif //MPI2_ABSTRACTNETWORK_H
