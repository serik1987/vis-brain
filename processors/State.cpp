//
// Created by serik1987 on 24.11.2019.
//

#include <stack>
#include "State.h"
#include "Equation.h"
#include "../log/output.h"

namespace equ{

    void State::calculateDerivative(int derivativeIndex, int equationIndex, double t, Ode::BufferType equationBuffer) {
        double real_t = t * getSolutionParameters().getIntegrationStep();
        for (auto it = begin(); it != end(); ++it){
            auto* equ = dynamic_cast<Equation*>(*it);
            if (equ != nullptr){
                if (equ->getFlag(Processor::IsInDerivative)){
                    equ->update(real_t);
                }
            }
            auto* diff = dynamic_cast<SingleOde*>(*it);
            if (diff != nullptr){
                diff->calculateDerivative(derivativeIndex, equationIndex, t, equationBuffer);
                diff->setCurrentOutput(equationIndex);
            }
        }
    }

    void State::increment(int outputNumber, int inputNumber, int derivativeNumber, double incrementStep,
                          Ode::BufferType inputBuffer, Ode::BufferType equationBuffer) {
        for (auto diff: diffList){
            diff->increment(outputNumber, inputNumber, derivativeNumber, incrementStep, inputBuffer, equationBuffer);
        }
    }

    void State::swap(int outputIndex1, int outputIndex2) {
        for (auto diff: diffList){
            diff->swap(outputIndex1, outputIndex2);
        }
    }

    void State::copy(int dest, int source) {
        for (auto diff: diffList){
            diff->copy(dest, source);
        }
    }

    void State::swapDerivative(int derivativeIndex1, int derivativeIndex2) {
        logging::enter();
        logging::debug("STATE SWAP DERIVATIVE");
        logging::exit();
    }

    void State::copyDerivative(int dest, int source) {
        logging::enter();
        logging::debug("STATE COPY DERIVATIVE");
        logging::exit();
    }

    void State::fillBuffer() {
        logging::enter();
        logging::debug("FILL BUFFER");
        logging::exit();
    }

    double State::getOutputDiscrepancy(int index1, int index2, Ode::BufferType buffer1, Ode::BufferType buffer2) {
        logging::enter();
        logging::debug("STATE GET OUTPUT DISCREPANCY");
        logging::exit();
        return 0;
    }

    void State::update(double time) {
        for (auto it = begin(); it != end(); ++it){
            auto equ = dynamic_cast<Equation*>(*it);
            if (equ != nullptr){
                if (equ->getFlag(Processor::IsInUpdate)){
                    equ->update(time);
                }
            }
            auto ode = dynamic_cast<SingleOde*>(*it);
            if (ode != nullptr){
                ode->update(time);
                ode->setCurrentOutput(0);
            }
        }
    }

    void State::printProcessorList() {
        logging::enter();
        logging::debug("Main processor list");
        for (auto proc: *this){
            logging::debug(proc->getName());
        }
        logging::debug("Differential equations list");
        for (auto equ: diffList){
            logging::debug(equ->getName());
        }
        logging::exit();
    }

    void State::addProcessor(Processor *proc) {
        std::stack<ProcessorStackItem> stack;
        ProcessorStackItem initial = {proc, proc->inputProcessorBegin()};
        stack.push(initial);
        int odeIndex = 0;
        {
            auto *ode = dynamic_cast<SingleOde *>(proc);
            if (ode != nullptr) {
                odeIndex++;
            }
        }
        while (!stack.empty()){
            ProcessorStackItem& current = stack.top();
            if (current.position == current.processor->inputProcessorEnd()){ /* No input processors not added */
                if (!current.processor->getFlag(Processor::AlreadyAdded)) {
                    push_back(current.processor);
                    auto* ode = dynamic_cast<SingleOde*>(current.processor);
                    if (ode != nullptr){
                        diffList.push_back(ode);
                        odeIndex--;
                    }
                    auto* equ = dynamic_cast<Equation*>(current.processor);
                    if (equ != nullptr){
                        if (odeIndex){
                            equ->setFlag(Processor::IsInDerivative, true);
                        } else {
                            equ->setFlag(Processor::IsInUpdate, true);
                        }
                    }
                    current.processor->setFlag(Processor::AlreadyAdded, true);
                }
                stack.pop();
            } else {
                Processor* input = *current.position;
                ++current.position;
                if (input != nullptr){
                    ProcessorStackItem new_item = {input, input->inputProcessorBegin()};
                    auto* ode = dynamic_cast<SingleOde*>(input);
                    if (ode != nullptr){
                        odeIndex++;
                    }
                    stack.push(new_item);
                }
            }
        }
    }

    void State::initialize(){
        for (auto proc: *this){
            proc->initialize();
        }
    }

    void State::finalize(){
        for (auto proc: *this){
            proc->finalize();
        }
    };

    State::~State(){
        for (auto proc: *this){
            delete proc;
        }
    }


}
