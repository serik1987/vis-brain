//
// Created by serik1987 on 20.11.2019.
//

#include "OdeTemporalKernel.h"
#include "../../../log/output.h"
#include "../../../data/reader/BinReader.h"

namespace equ{

    void OdeTemporalKernel::loadParameterList(const param::Object &source) {
        using std::to_string;
        logging::info("Temporal kernel parameters:");
        logging::info("ODE temporal kernel was applied");
        setTimeConstant(source.getFloatField("tau"));
        logging::info("Ascending time constant, ms: " + to_string(getTimeConstant()));
        setLateTimeConstant(source.getFloatField("tau_late"));
        logging::info("Descending time constant, ms: " + to_string(getLateTimeConstant()));
        setK(source.getFloatField("K"));
        logging::info("K = " + to_string(getK()));
        setInitialStimulusValue(source.getFloatField("initial_stimulus_value"));
        logging::info("Initial stimulus value: " + to_string(getInitialStimulusValue()));
    }

    void OdeTemporalKernel::broadcastParameterList() {
        Application::getInstance().broadcastDouble(timeConstant, 0);
        setTimeConstant(timeConstant);
        Application::getInstance().broadcastDouble(lateTimeConstant, 0);
        setLateTimeConstant(lateTimeConstant);
        Application::getInstance().broadcastDouble(K, 0);
        Application::getInstance().broadcastDouble(initialStimulusValue, 0);
    }

    void OdeTemporalKernel::setParameter(const std::string &name, const void *pvalue) {
        if (name == "tau") {
            setTimeConstant(*(double *) pvalue);
        } else if (name == "tau_late") {
            setLateTimeConstant(*(double*)pvalue);
        } else if (name == "initial_stimulus_value"){
            setInitialStimulusValue(*(double*)pvalue);
        } else {
            throw param::IncorrectParameterName(name, "ODE temporal kernel");
        }
    }

    void OdeTemporalKernel::update(double time) {
        auto m_early = getOutput(EQUATION_m, 0).begin();
        auto m_early_final = getOutput(EQUATION_m, 0).end();
        auto m_late = getOutput(EQUATION_m_LATE, 0).begin();

        for (; m_early != m_early_final; ++m_early, ++m_late){
            *m_early -= getK() * *m_late;
        }
    }

    void OdeTemporalKernel::initializeSingleOde() {
        getOutput(EQUATION_U, 0).fill(getInitialStimulusValue());
        getOutput(EQUATION_m, 0).fill(getInitialStimulusValue());
        getOutput(EQUATION_U_LATE, 0).fill(getInitialStimulusValue());
        getOutput(EQUATION_m_LATE, 0).fill(getInitialStimulusValue());
    }

    void OdeTemporalKernel::calculateDerivative(int derivativeIndex, int equationIndex, double t,
                                                Ode::BufferType equationBuffer) {
        double tau = getSolutionParameters().getTimeConstant();
        double tau_late = lateTimeConstantH;

        auto dU_dt = SINGLE_ODE_DERIVATIVE(EQUATION_U);
        auto dU_dt_final = SINGLE_ODE_DERIVATIVE_LAST_PIXEL(EQUATION_U);
        auto I = getStimulusSaturation()->getOutput().begin();
        auto U = SINGLE_ODE_OUTPUT(EQUATION_U);
        auto dm_dt = SINGLE_ODE_DERIVATIVE(EQUATION_m);
        auto m = SINGLE_ODE_OUTPUT(EQUATION_m);

        auto dU_dt_late = SINGLE_ODE_DERIVATIVE(EQUATION_U_LATE);
        auto U_late = SINGLE_ODE_OUTPUT(EQUATION_U_LATE);
        auto dm_dt_late = SINGLE_ODE_DERIVATIVE(EQUATION_m_LATE);
        auto m_late = SINGLE_ODE_OUTPUT(EQUATION_m_LATE);

        for (; dU_dt != dU_dt_final; ++dU_dt, ++I, ++U, ++dm_dt, ++m, ++dU_dt_late, ++U_late, ++dm_dt_late, ++m_late){
            *dU_dt = (*I - *U)/tau;
            *dm_dt = (*U - *m)/tau;
            *dU_dt_late = (*I - *U_late)/tau_late;
            *dm_dt_late = (*U_late - *m_late)/tau_late;
        }

    }
}