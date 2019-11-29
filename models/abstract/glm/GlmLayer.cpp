//
// Created by serik1987 on 18.11.2019.
//

#include "GlmLayer.h"
#include "../../../log/output.h"

namespace net{

    equ::Processor *GlmLayer::createGlmMechanism(mpi::Communicator &comm, const std::string &mechanism,
            equ::Ode::SolutionParameters parameters) {
        using std::string;
        equ::Processor* instance = nullptr;

        size_t delimiter = mechanism.find('.');
        string major_name;
        string minor_name;
        if (delimiter == string::npos){
            major_name = mechanism;
            minor_name = "";
        } else {
            major_name = mechanism.substr(0, delimiter);
            minor_name = mechanism.substr(delimiter + 1);
        }

        if (major_name == "stimulus_saturation") {
            instance = equ::StimulusSaturation::createStimulusSaturation(comm, minor_name);
        } else if (major_name == "temporal_kernel") {
            instance = equ::TemporalKernel::createTemporalKernel(comm, minor_name, parameters);
        } else if (major_name == "spatial_kernel") {
            instance = equ::SpatialKernel::createSpatialKernel(comm, minor_name);
        } else if (major_name == "dog"){
            instance = new equ::DogFilter(comm);
        } else {
            throw param::UnknownMechanism("glm:" + mechanism);
        }

        return instance;
    }

    GlmLayer::~GlmLayer() {
        deleteProcessors();
    }

    void GlmLayer::loadAbstractModelParameters(const param::Object &source) {
        if (getDeepFlag()) {
            equ::Processor *current_processor = nullptr;

            auto saturation_parameters = source.getObjectField("saturation");
            saturation_mechanism = saturation_parameters.getStringField("mechanism");
            current_processor = equ::Processor::createProcessor(getCommunicator(), saturation_mechanism,
                    getMethod().getSolutionParameters());
            saturation = dynamic_cast<equ::StimulusSaturation*>(current_processor);
            if (saturation == nullptr){
                throw IncorrectSaturationProperty();
            }
            saturation->loadParameters(saturation_parameters);

            logging::info("Excitatory kernel parameters");

            auto excitation_parameters = source.getObjectField("excitation");
            auto excitatory_temporal_kernel_parameters = excitation_parameters.getObjectField("temporal_kernel");
            excitatory_temporal_kernel_mechanism = excitatory_temporal_kernel_parameters.getStringField("mechanism");
            current_processor = equ::Processor::createProcessor(getCommunicator(),
                    excitatory_temporal_kernel_mechanism, getMethod().getSolutionParameters());
            excitatory_temporal_kernel = dynamic_cast<equ::TemporalKernel*>(current_processor);
            if (excitatory_temporal_kernel == nullptr){
                throw IncorrectExcitatoryTemporalKernel();
            }
            excitatory_temporal_kernel->setStimulusSaturation(saturation);
            excitatory_temporal_kernel->loadParameters(excitatory_temporal_kernel_parameters);

            auto excitatory_spatial_kernel_parameters = excitation_parameters.getObjectField("spatial_kernel");
            excitatory_spatial_kernel_mechanism = excitatory_spatial_kernel_parameters.getStringField("mechanism");
            current_processor = equ::Processor::createProcessor(getCommunicator(),
                    excitatory_spatial_kernel_mechanism, getMethod().getSolutionParameters());
            excitatory_spatial_kernel = dynamic_cast<equ::SpatialKernel*>(current_processor);
            if (excitatory_spatial_kernel == nullptr){
                throw IncorrectExcitatorySpatialKernel();
            }
            excitatory_spatial_kernel->setTemporalKernel(excitatory_temporal_kernel);
            excitatory_spatial_kernel->loadParameters(excitatory_spatial_kernel_parameters);

            logging::info("Inhibitory kernel parameters");
            auto inhibitory_parameters = source.getObjectField("inhibition");
            auto inhibitory_temporal_kernel_parameters = inhibitory_parameters.getObjectField("temporal_kernel");
            inhibitory_temporal_kernel_mechanism = inhibitory_temporal_kernel_parameters.getStringField("mechanism");
            current_processor = equ::Processor::createProcessor(getCommunicator(), inhibitory_temporal_kernel_mechanism,
                    getMethod().getSolutionParameters());
            inhibitory_temporal_kernel = dynamic_cast<equ::TemporalKernel*>(current_processor);
            if (inhibitory_temporal_kernel == nullptr){
                throw IncorrectInhibitoryTemporalKernel();
            }
            inhibitory_temporal_kernel->setStimulusSaturation(saturation);
            inhibitory_temporal_kernel->loadParameters(inhibitory_temporal_kernel_parameters);

            auto inhibitory_spatial_kernel_parameters = inhibitory_parameters.getObjectField("spatial_kernel");
            inhibitory_spatial_kernel_mechanism = inhibitory_spatial_kernel_parameters.getStringField("mechanism");
            current_processor = equ::Processor::createProcessor(getCommunicator(), inhibitory_spatial_kernel_mechanism,
                    getMethod().getSolutionParameters());
            inhibitory_spatial_kernel = dynamic_cast<equ::SpatialKernel*>(current_processor);
            if (inhibitory_spatial_kernel == nullptr){
                throw IncorrectInhibitorySpatialKernel();
            }
            inhibitory_spatial_kernel->setTemporalKernel(inhibitory_temporal_kernel);
            inhibitory_spatial_kernel->loadParameters(inhibitory_spatial_kernel_parameters);

            auto dog_filter_parameters = source.getObjectField("dog_filter");
            dog_filter_mechanism = dog_filter_parameters.getStringField("mechanism");
            current_processor = equ::Processor::createProcessor(getCommunicator(), dog_filter_mechanism,
                    getMethod().getSolutionParameters());
            dog_filter = dynamic_cast<equ::DogFilter*>(current_processor);
            if (dog_filter == nullptr){
                throw IncorrectDogFilter();
            }
            dog_filter->setSpatialKernels(excitatory_spatial_kernel, inhibitory_spatial_kernel);
            dog_filter->loadParameters(dog_filter_parameters);
        }
    }

    void GlmLayer::broadcastAbstractModelParameters() {
        if (getDeepFlag()) {
            auto& app = Application::getInstance();
            app.broadcastString(saturation_mechanism, 0);
            app.broadcastString(excitatory_temporal_kernel_mechanism, 0);
            app.broadcastString(excitatory_spatial_kernel_mechanism, 0);
            app.broadcastString(inhibitory_temporal_kernel_mechanism, 0);
            app.broadcastString(inhibitory_spatial_kernel_mechanism, 0);
            app.broadcastString(dog_filter_mechanism, 0);

            if (app.getAppCommunicator().getRank() != 0) {
                equ::Processor *current_processor;
                current_processor = equ::Processor::createProcessor(getCommunicator(), saturation_mechanism,
                                                                    getMethod().getSolutionParameters());
                saturation = dynamic_cast<equ::StimulusSaturation *>(current_processor);

                current_processor = equ::Processor::createProcessor(getCommunicator(),
                        excitatory_temporal_kernel_mechanism, getMethod().getSolutionParameters());
                excitatory_temporal_kernel = dynamic_cast<equ::TemporalKernel*>(current_processor);
                excitatory_temporal_kernel->setStimulusSaturation(saturation);

                current_processor = equ::Processor::createProcessor(getCommunicator(),
                        excitatory_spatial_kernel_mechanism, getMethod().getSolutionParameters());
                excitatory_spatial_kernel = dynamic_cast<equ::SpatialKernel*>(current_processor);
                excitatory_spatial_kernel->setTemporalKernel(excitatory_temporal_kernel);

                current_processor = equ::Processor::createProcessor(getCommunicator(),
                        inhibitory_temporal_kernel_mechanism, getMethod().getSolutionParameters());
                inhibitory_temporal_kernel = dynamic_cast<equ::TemporalKernel*>(current_processor);
                inhibitory_temporal_kernel->setStimulusSaturation(saturation);

                current_processor = equ::Processor::createProcessor(getCommunicator(),
                        inhibitory_spatial_kernel_mechanism, getMethod().getSolutionParameters());
                inhibitory_spatial_kernel = dynamic_cast<equ::SpatialKernel*>(current_processor);
                inhibitory_spatial_kernel->setTemporalKernel(inhibitory_temporal_kernel);

                current_processor = equ::Processor::createProcessor(getCommunicator(),
                        dog_filter_mechanism, getMethod().getSolutionParameters());
                dog_filter = dynamic_cast<equ::DogFilter*>(current_processor);
                dog_filter->setSpatialKernels(excitatory_spatial_kernel, inhibitory_spatial_kernel);
            }

            saturation->broadcastParameters();
            excitatory_temporal_kernel->broadcastParameters();
            excitatory_spatial_kernel->broadcastParameters();
            inhibitory_temporal_kernel->broadcastParameters();
            inhibitory_spatial_kernel->broadcastParameters();
            dog_filter->broadcastParameters();

            if (getStatus() != Active){
                deleteProcessors();
            }
            /*
            logging::enter();
            logging::debug("GLM model pipeline");
            if (dog_filter != nullptr) {
                dog_filter->printAllProcessors(0, app.getAppCommunicator().getRank());
            }
            logging::exit();
             */
        }
    }

    void GlmLayer::deleteProcessors(){
        delete saturation;
        saturation = nullptr;
        delete excitatory_temporal_kernel;
        excitatory_temporal_kernel = nullptr;
        delete excitatory_spatial_kernel;
        excitatory_spatial_kernel = nullptr;
        delete inhibitory_temporal_kernel;
        inhibitory_temporal_kernel = nullptr;
        delete inhibitory_spatial_kernel;
        inhibitory_spatial_kernel = nullptr;
        delete dog_filter;
        dog_filter = nullptr;
    }

    void GlmLayer::immediateAddToState(equ::State &state) {
        state.addProcessor(dog_filter);
    }
}