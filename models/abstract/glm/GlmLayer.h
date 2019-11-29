//
// Created by serik1987 on 18.11.2019.
//

#ifndef MPI2_GLMLAYER_H
#define MPI2_GLMLAYER_H

#include "../AbstractModel.h"
#include "../../../processors/Processor.h"

#include "StimulusSaturation.h"
#include "TemporalKernel.h"
#include "SpatialKernel.h"
#include "DogFilter.h"

namespace net {

    /**
     * Layer that describes by the GLM model
     */
    class GlmLayer: public AbstractModel {
    private:
        std::string saturation_mechanism, excitatory_temporal_kernel_mechanism, inhibitory_temporal_kernel_mechanism,
        excitatory_spatial_kernel_mechanism, inhibitory_spatial_kernel_mechanism, dog_filter_mechanism;

        equ::StimulusSaturation *saturation = nullptr;
        equ::TemporalKernel *excitatory_temporal_kernel = nullptr, *inhibitory_temporal_kernel = nullptr;
        equ::SpatialKernel *excitatory_spatial_kernel = nullptr, *inhibitory_spatial_kernel = nullptr;
        equ::DogFilter* dog_filter;

        void deleteProcessors();

    protected:
        [[nodiscard]] std::string getLayerDescription() override { return "GLM model"; }

        void loadAbstractModelParameters(const param::Object& source) override;
        void broadcastAbstractModelParameters() override;

        void immediateAddToState(equ::State& state) override;

        equ::Processor* getLayerInputData() override { return saturation; }


    public:
        explicit GlmLayer(const std::string& name, const std::string& parent = ""):
            AbstractModel(name, parent) {};

        ~GlmLayer() override;

        /**
         * Creates a certain child mechanism that is constituent of the GLM model
         *
         * @param comm communicator responsible for the mechanism
         * @param mechanism mechanism name (without glm: prefix)
         * @param parameters object generated by the solution method
         * @return instance to the processor
         */
        static equ::Processor* createGlmMechanism(mpi::Communicator& comm, const std::string& mechanism,
                equ::Ode::SolutionParameters parameters);

        class IncorrectSaturationProperty: public std::exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "the property 'saturation' of the GLM model doesn't refer to the valid saturation mechanism";
            }
        };

        class IncorrectExcitatoryTemporalKernel: public std::exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "property 'excitation.temporal_kernel' of the GLM model doesn't refer to the "
                       "valid temporal kernel";
            }
        };

        class IncorrectExcitatorySpatialKernel: public std::exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "property 'excitation.spatial' of the GLM model doesn't refer to the "
                       "valid spatial kernel";
            }
        };

        class IncorrectInhibitoryTemporalKernel: public std::exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "property 'inhibition.temporal_kernel' of the GLM model doesn't refer to the "
                       "valid temporal kernel";
            }
        };

        class IncorrectInhibitorySpatialKernel: public std::exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "property 'inhibition.spatial' of the GLM model doesn't refer to the "
                       "valid spatial kernel";
            }
        };

        class IncorrectDogFilter: public std::exception{
        public:
            [[nodiscard]] const char* what() const noexcept override{
                return "property 'dog_filter' ofvirtual the GLM model doesn't refer to the valid DOG filter";
            }
        };

        /**
         *
         * @return processor that represents the output data from the layer
         */
        equ::Processor* getOutputData() override { return dog_filter; }
    };

}


#endif //MPI2_GLMLAYER_H
