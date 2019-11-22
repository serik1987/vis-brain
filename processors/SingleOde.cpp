//
// Created by serik1987 on 20.11.2019.
//

#include "SingleOde.h"
#include "../log/output.h"

namespace equ{

    void SingleOde::initialize(){
        if (initialized) return;
        SolutionParameters par = getSolutionParameters();
        for (int btype = 0; btype < 1 + par.isDoubleBuffer(); ++btype){
            Buffer* buffer = new Buffer(par.getEquationNumber());
            for (int i=0; i < par.getEquationNumber(); ++i){
                buffer->at(i).out = initializeLine(par.getEquationOrder() + 1);
                buffer->at(i).der = initializeLine(par.getDerivativeOrder());
            }
            buffers[btype] = buffer;
        }
        initializeSingleOde();
        output = buffers[PublicBuffer]->at(getMainEquation()).der->at(0);
        currentOutput = 0;
        initialized = true;
    }

    SingleOde::Line* SingleOde::initializeLine(int matrixNumber) {
        Line* line = new Line(matrixNumber);
        for (size_t i=0; i < line->size(); ++i){
            line->at(i) = new data::LocalMatrix(getCommunicator(),
                    getGridX(), getGridY(), getSizeX(), getSizeY());
        }
        return line;
    }

    void SingleOde::finalizeProcessor(bool destruct) noexcept {
        if (!initialized) return;
        output = nullptr;
        currentOutput = -1;
        SolutionParameters par = getSolutionParameters();
        for (int btype = 0; btype < 1 + par.isDoubleBuffer(); ++btype){
            Buffer* buffer = buffers[btype];
            for (int i=0; i < par.getEquationNumber(); ++i){
                finalizeLine(buffer->at(i).out);
                finalizeLine(buffer->at(i).der);
            }
            delete buffers[btype];
        }
        if (!destruct) finalizeSingleOde(false);
        initialized = false;
    }

    void SingleOde::finalizeLine(equ::SingleOde::Line *line) {
        for (size_t i = 0; i < line->size(); ++i){
            delete line->at(i);
        }
        delete line;
    }

    void SingleOde::increment(int outputNumber, int inputNumber, int derivativeNumber, double incrementStep,
                              Ode::BufferType inputBuffer, Ode::BufferType equationBuffer) {
        for (int i=0; i < getSolutionParameters().getEquationNumber(); ++i){
            data::LocalMatrix& out = *buffers[PublicBuffer]->at(i).out->at(outputNumber);
            data::LocalMatrix& in = *buffers[inputBuffer]->at(i).out->at(inputNumber);
            data::LocalMatrix& der = *buffers[equationBuffer]->at(i).der->at(derivativeNumber);
            auto out_it = out.begin();
            auto in_it = in.cbegin();
            auto der_it = der.cbegin();
            for (; out_it != out.end(); out_it++, in_it++, der_it++){
                *out_it = *in_it + incrementStep * *der_it;
            }
        }
    }

    void SingleOde::swap(int outputIndex1, int outputIndex2) {
        for (int i=0; i < getSolutionParameters().getEquationNumber(); ++i){
            std::swap(buffers[PublicBuffer]->at(i).out->at(outputIndex1),
                    buffers[PublicBuffer]->at(i).out->at(outputIndex2));
        }
    }

    void SingleOde::copy(int dest, int source) {
        for (int i=0; i < getSolutionParameters().getEquationNumber();  ++i){
            data::LocalMatrix& mdest = *buffers[PublicBuffer]->at(i).out->at(dest);
            data::LocalMatrix& msource = *buffers[PublicBuffer]->at(i).out->at(source);
            auto idest = mdest.begin();
            auto isource = msource.cbegin();
            for (; idest != mdest.end(); ++idest, ++isource){
                *idest = *isource;
            }
        }
    }

    void SingleOde::swapDerivative(int derivativeIndex1, int derivativeIndex2) {
        logging::enter();
        logging::debug("SWAP DERIVATIVE");
        logging::exit();
    }

    void SingleOde::copyDerivative(int dest, int source) {
        logging::enter();
        logging::debug("COPY DERIVATIVE");
        logging::exit();
    }

    void SingleOde::fillBuffer() {
        logging::enter();
        logging::debug("FILL BUFFER");
        logging::exit();
    }

    double SingleOde::getOutputDiscrepancy(int index1, int index2, Ode::BufferType buffer1, Ode::BufferType buffer2) {
        logging::enter();
        logging::debug("GET OUTPUT DISCREPANCY");
        logging::exit();
        return 0;
    }

    void SingleOde::setCurrentOutput(int index) {
        currentOutput = index;
    }
}