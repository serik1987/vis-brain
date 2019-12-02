//
// Created by serik1987 on 27.11.2019.
//

#ifndef MPI2_BRAIN_H
#define MPI2_BRAIN_H

#include "Network.h"

namespace net {

    class Brain: public Network {
    public:
        Brain(): Network("brain") {};

        /**
         * Creates processors for the brain after the distributor will be applied.
         * If the distributor is not applied, the behaviour of the method is unpredictable
         */
        void createProcessors();

    };

}


#endif //MPI2_BRAIN_H
