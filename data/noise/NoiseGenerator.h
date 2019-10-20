//
// Created by serik1987 on 20.10.2019.
//

#ifndef MPI2_NOISEGENERATOR_H
#define MPI2_NOISEGENERATOR_H

#include "../Matrix.h"

#include <random>

namespace data::noise {


    class NoiseGenerator {
    private:
        static bool initialized;
    public:
        NoiseGenerator() {
            if (!initialized){
                std::srand(unsigned(time(nullptr)) + Application::getInstance().getAppCommunicator().getRank());
            }
        }

        virtual double getValue() = 0;

        virtual void fill(data::Matrix& matrix);
    };

}


#endif //MPI2_NOISEGENERATOR_H
