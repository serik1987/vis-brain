//
// Created by serik1987 on 20.10.2019.
//

#ifndef MPI2_UNIFORMNOISEGENERATOR_H
#define MPI2_UNIFORMNOISEGENERATOR_H

#include "NoiseGenerator.h"

namespace data::noise {

    class UniformNoiseGenerator : public NoiseGenerator {
    private:
        double min;
        double max;
    public:
        UniformNoiseGenerator(double min, double max): NoiseGenerator(), min(min), max(max) {};

        double getValue() override final {
            return min + (double)rand() * (max - min) / RAND_MAX;
        }
    };

}

#endif //MPI2_UNIFORMNOISEGENERATOR_H
