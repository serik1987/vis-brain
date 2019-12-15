//
// Created by serik1987 on 03.12.2019.
//

#include "SecondaryVsdAnalyzer.h"

namespace analysis{

    int SecondaryVsdAnalyzer::getMatrixWidth() {
        return getSource().getWidth();
    }

    int SecondaryVsdAnalyzer::getMatrixHeight() {
        return getSource().getHeight();
    }

    double SecondaryVsdAnalyzer::getMatrixWidthUm() {
        return getSource().getWidthUm();
    }

    double SecondaryVsdAnalyzer::getMatrixHeightUm() {
        return getSource().getHeightUm();
    }

    void SecondaryVsdAnalyzer::checkSource(Analyzer *source) {
        auto* an = dynamic_cast<VsdAnalyzer*>(source);
        if (an == nullptr){
            throw incorrect_input_analyzer();
        }
        dt = an->getAcquisitionStep();
    }
}