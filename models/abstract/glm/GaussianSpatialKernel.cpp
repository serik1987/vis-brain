//
// Created by serik1987 on 23.11.2019.
//

#include "GaussianSpatialKernel.h"
#include "../../../log/output.h"

namespace equ{

    void GaussianSpatialKernel::loadParameterList(const param::Object &source) {
        logging::info("Spatial kernel parameters");
        setRadius(source.getFloatField("radius"));
        logging::info("Spatial kernel radius: " + std::to_string(getRadius()));
    }

    void GaussianSpatialKernel::broadcastParameterList() {
        Application& app = Application::getInstance();
        app.broadcastDouble(radius, 0);
    }

    void GaussianSpatialKernel::setParameter(const std::string &name, const void *pvalue) {
        if (name  == "radius") {
            setRadius(*(double*)pvalue);
        } else {
            throw param::IncorrectParameterName(name, "gaussian spatial kernel");
        }
    }

    void GaussianSpatialKernel::initializeSpatialKernel() {
        auto& input = getTemporalKernel()->getOutput();
        double resX = input.getWidthUm() / (input.getWidth() - 1);
        double resY = input.getHeightUm() / (input.getHeight() - 1);
        double r = getRadius();
        double size = 4 * r;
        double kernelWidthUm = size;
        double kernelHeightUm = size;
        int kernelWidth = (int)round(kernelWidthUm / resX) + 1;
        int kernelHeight = (int)round(kernelHeightUm / resY) + 1;
        kernel = new data::ContiguousMatrix(getCommunicator(), kernelWidth, kernelHeight,
                kernelWidthUm, kernelHeightUm, 20.0);

        for (auto kpix = kernel->begin(); kpix != kernel->end(); ++kpix){
            double x = kpix.getColumnUm();
            double y = kpix.getRowUm();
            *kpix = exp(-(x*x)/(r*r) - (y*y)/(r*r));
        }
    }
}