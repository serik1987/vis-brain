//
// Created by serik1987 on 20.11.2019.
//

#include "OdeTemporalKernel.h"
#include "../../../log/output.h"
#include "../../../data/reader/BinReader.h"

namespace equ{

    void OdeTemporalKernel::loadParameterList(const param::Object &source) {
        setTimeConstant(source.getFloatField("tau"));
        setInitialStimulusValue(source.getFloatField("initial_stimulus_value"));
    }

    void OdeTemporalKernel::broadcastParameterList() {
        Application::getInstance().broadcastDouble(timeConstant, 0);
        setTimeConstant(timeConstant);
        Application::getInstance().broadcastDouble(initialStimulusValue, 0);
    }

    void OdeTemporalKernel::setParameter(const std::string &name, const void *pvalue) {
        if (name == "tau") {
            setTimeConstant(*(double *) pvalue);
        } else if (name == "initial_stimulus_value"){
            setInitialStimulusValue(*(double*)pvalue);
        } else {
            throw param::IncorrectParameterName(name, "ODE temporal kernel");
        }
    }

    void OdeTemporalKernel::update(double time) {}

    void OdeTemporalKernel::initializeSingleOde() {
        getOutput(0, 0).fill(getInitialStimulusValue());
        getOutput(1, 0).fill(getInitialStimulusValue());
    }

    void OdeTemporalKernel::calculateDerivative(int derivativeIndex, int equationIndex, double t,
                                                Ode::BufferType equationBuffer) {
        double tau = getSolutionParameters().getTimeConstant();

        auto dU_dt = getDerivative(0, derivativeIndex).begin();
        auto dU_dt_final = getDerivative(0, derivativeIndex).end();
        auto I = getStimulusSaturation()->getOutput().begin();
        auto U = getOutput(0, equationIndex, equationBuffer).begin();
        auto dm_dt = getDerivative(1, derivativeIndex).begin();
        auto m = getOutput(1, equationIndex, equationBuffer).begin();

        for (; dU_dt != dU_dt_final; ++dU_dt, ++I, ++U, ++dm_dt, ++m){
            *dU_dt = (*I - *U)/tau;
            *dm_dt = (*U - *m)/tau;
        }

    }
}