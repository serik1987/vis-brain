//
// Created by serik1987 on 01.12.2019.
//

#include "PrimaryVsdAnalyzer.h"
#include "../log/output.h"


namespace analysis{

    void PrimaryVsdAnalyzer::loadAnalysisParameters(const param::Object &source) {
        setImagingAreaSizeX(source.getFloatField("imaging_area_size_x"));
        setImagingAreaSizeY(source.getFloatField("imaging_area_size_y"));
        setAcquisitionStep(source.getFloatField("acquisition_step"));
    }

    void PrimaryVsdAnalyzer::broadcastAnalysisParameters() {
        Application& app = Application::getInstance();
        app.broadcastDouble(imaging_area_size_x, 0);
        app.broadcastDouble(imaging_area_size_y, 0);
        app.broadcastDouble(acquisition_step, 0);
    }

    void PrimaryVsdAnalyzer::setAnalyzerParameters(const std::string &name, const void *pvalue) {
        if (name == "imaging_area_size_x") {
            setImagingAreaSizeX(*(double *) pvalue);
        } else if (name == "imaging_area_size_y") {
            setImagingAreaSizeY(*(double *) pvalue);
        } else if (name == "acquisition_step"){
            setAcquisitionStep(*(double*)pvalue);
        } else {
            throw param::IncorrectParameterName(name, "primary VSD analyser");
        }
    }

    void PrimaryVsdAnalyzer::initializeVsd() {
        if (abs(getMatrixWidthUm() - getImagingAreaSizeX()) > 1e-8){
            std::stringstream ss;
            ss << "Actual imaging area horizontal size is " << getMatrixWidthUm() <<
            " because the resolution shall be the same number of pixels on horizontal shall be integer";
            logging::enter();
            logging::warning(ss.str());
            logging::exit();
        }
        if (abs(getMatrixHeightUm() - getImagingAreaSizeY()) > 1e-8){
            std::stringstream ss;
            ss << "Actual imaging area vertical size is " << getMatrixHeightUm() <<
               " because the resolution shall be the same number of pixels on vertical shall be integer";
            logging::enter();
            logging::warning(ss.str());
            logging::exit();
        }

        mpi::Communicator& comm = getInputCommunicator();
        int width = getSource().getWidth();
        int height = getSource().getHeight();
        double widthUm = getSource().getWidthUm();
        double heightUm = getSource().getHeightUm();
        buffer = new data::ContiguousMatrix(comm, width, height, widthUm, heightUm, 0.0);

        double dt = Application::getInstance().getMethod().getIntegrationTime();
        acquisition_ts = (unsigned long long)std::round(getAcquisitionStep() / dt);
    }

    void PrimaryVsdAnalyzer::finalizeProcessor(bool destruct) noexcept {
        delete buffer;
        buffer = nullptr;
    }

    void PrimaryVsdAnalyzer::update(double time) {
        auto it_source = getSource().begin();
        auto buf_source = buffer->begin();
        for (; buf_source != buffer->end(); ++buf_source, ++it_source){
            *buf_source = *it_source;
        }

        buffer->synchronize();

        for (auto it = output->begin(); it != output->end(); ++it){
            int i = it.getRow();
            int j = it.getColumn();
            int I = (buffer->getHeight() - output->getHeight())/2 + i;
            int J = (buffer->getWidth() - output->getWidth())/2 + j;
            data::ContiguousMatrix::ConstantIterator src(*buffer, I, J);
            *it = *src;
        }
    }

    int PrimaryVsdAnalyzer::getMatrixWidth() {
        using std::round;
        int i = (int)round((getSource().getWidth() - 1) * getImagingAreaSizeX() / getSource().getWidthUm()) + 1;
        if (i > getSource().getWidth()) i = getSource().getWidth();
        return i;
    }

    int PrimaryVsdAnalyzer::getMatrixHeight() {
        using std::round;
        int j = (int)round((getSource().getHeight() - 1) * getImagingAreaSizeY() / getSource().getHeightUm()) + 1;
        if (j > getSource().getHeight()) j = getSource().getHeight();
        return j;
    }

    double PrimaryVsdAnalyzer::getMatrixWidthUm() {
        using std::round;
        double w = 0.0;
        w = getSource().getWidthUm() *
                round((getSource().getWidth()-1) * getImagingAreaSizeX() / getSource().getWidthUm())
                / (getSource().getWidth() - 1);
        if (w > getSource().getWidthUm()) w = getSource().getWidthUm();
        return w;
    }

    double PrimaryVsdAnalyzer::getMatrixHeightUm() {
        using std::round;
        double h = 0.0;
        h = getSource().getHeightUm() *
                round((getSource().getHeight()-1) * getImagingAreaSizeX() / getSource().getHeightUm())
                / (getSource().getHeight() - 1);
        if (h > getSource().getHeightUm()) h = getSource().getHeightUm();
        return h;
    }

    bool PrimaryVsdAnalyzer::isReady(double time) {
        double itime = Application::getInstance().getMethod().getIntegrationTime();
        auto current_ts = (unsigned long long)(time / itime);
        return current_ts % acquisition_ts == 0;
    }
}