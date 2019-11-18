//
// Created by serik1987 on 18.11.2019.
//

#ifndef MPI2_GLMLAYER_H
#define MPI2_GLMLAYER_H

#include "../../../processors/Processor.h"

namespace net {

    class GlmLayer {
    public:
        static equ::Processor* createGlmMechanism(mpi::Communicator& comm, const std::string& mechanism);
    };

}


#endif //MPI2_GLMLAYER_H
