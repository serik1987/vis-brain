//
// Created by serik1987 on 05.11.2019.
//

#include "noise.h"

namespace data::noise{

    void WhiteNoise::update(){
        for (auto& pixel: matrix){
            pixel = distribution();
        }
    }

    void BrownNoise::update(){
        for (auto& pixel: matrix){
            pixel += k * distribution();
        }
    }

}