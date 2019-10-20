//
// Created by serik1987 on 20.10.2019.
//

#include "NoiseGenerator.h"

namespace data::noise {

    bool NoiseGenerator::initialized = false;

    void NoiseGenerator::fill(data::Matrix& matrix){
        for (auto a = matrix.begin(); a != matrix.end(); ++a){
            *a = getValue();
        }
    }

}