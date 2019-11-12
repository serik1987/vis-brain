//
// Created by serik1987 on 11.11.2019.
//

#include "Processor.h"

namespace equ{

    void Processor::finalize(bool destruct) noexcept{
        delete output;
        output = nullptr;
        if (!destruct) {
            finalizeProcessor();
        }
    }

}