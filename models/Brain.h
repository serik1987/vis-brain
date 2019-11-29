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

    };

}


#endif //MPI2_BRAIN_H
