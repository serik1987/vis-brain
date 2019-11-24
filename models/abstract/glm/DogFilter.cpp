//
// Created by serik1987 on 24.11.2019.
//

#include "DogFilter.h"
#include "../../../log/output.h"
#include "../../../data/LocalMatrix.h"

namespace equ{

    void DogFilter::loadParameterList(const param::Object &source) {
        logging::info("General properties of the spatiotemporal filter:");
        setDarkRate(source.getFloatField("dark_rate"));
        logging::info("Dark firing rate, Hz: " + std::to_string(getDarkRate()));
        setExcitatoryWeight(source.getFloatField("excitatory_weight"));
        logging::info("Excitatory weight: " + std::to_string(getExcitatoryWeight()));
        setInhibitoryWeight(source.getFloatField("inhibitory_weight"));
        logging::info("Inhibitory weight: " + std::to_string(getInhibitoryWeight()));
        setThreshold(source.getFloatField("threshold"));
        logging::info("Threshold, Hz: " + std::to_string(getThreshold()));
    }

    void DogFilter::broadcastParameterList() {
        Application& app = Application::getInstance();
        app.broadcastDouble(darkRate, 0);
        app.broadcastDouble(excitatoryWeight, 0);
        app.broadcastDouble(inhibitoryWeight, 0);
        app.broadcastDouble(threshold, 0);
    }

    void DogFilter::setParameter(const std::string &name, const void *pvalue) {
        if (name == "dark_rate") {
            setDarkRate(*(double *) pvalue);
        } else if (name == "excitatory_weight") {
            setExcitatoryWeight(*(double *) pvalue);
        } else if (name == "inhibitory_weight") {
            setInhibitoryWeight(*(double *) pvalue);
        } else if (name == "threshold"){
            setThreshold(*(double*)pvalue);
        } else {
            throw param::IncorrectParameterName(name, "dog filter");
        }
    }

    void DogFilter::initialize() {
        auto* exc = getExcitatoryKernel();
        auto* inh = getInhibitoryKernel();
        int width = exc->getOutput().getWidth();
        int height = exc->getOutput().getHeight();
        double widthUm = exc->getOutput().getWidthUm();
        double heightUm = exc->getOutput().getHeightUm();

        if (width != inh->getOutput().getWidth()){
            throw input_dimensions_mismatch();
        }

        if (height != inh->getOutput().getHeight()){
            throw input_dimensions_mismatch();
        }

        if (widthUm != inh->getOutput().getWidthUm()){
            throw input_dimensions_mismatch();
        }

        if (heightUm != inh->getOutput().getHeightUm()){
            throw input_dimensions_mismatch();
        }

        output = new data::LocalMatrix(getCommunicator(), width, height, widthUm, heightUm);
    }

    void DogFilter::update(double time) {
        auto r = output->begin();
        auto r_end = output->end();
        auto e = excitation->getOutput().cbegin();
        auto i = inhibition->getOutput().cbegin();
        double r0 = getDarkRate();
        double k_e = getExcitatoryWeight();
        double k_i = getInhibitoryWeight();
        double T = getThreshold();

        for (; r != r_end; ++r, ++e, ++i){
            *r = r0 + k_e * *e + k_i * *i;
            if (*r < T) *r = 0.0;
        }
    }

    void DogFilter::finalizeProcessor(bool destruct) noexcept {

    }

    void DogFilter::setSpatialKernels(SpatialKernel *excitatoryKernel, SpatialKernel *inhibitoryKernel) {
        while (getInputProcessorNumber() > 0){
            auto* proc = *inputProcessorBegin();
            removeInputProcessor(proc);
        }
        addInputProcessor(excitatoryKernel);
        addInputProcessor(inhibitoryKernel);
        excitation = excitatoryKernel;
        inhibition = inhibitoryKernel;
    }
}