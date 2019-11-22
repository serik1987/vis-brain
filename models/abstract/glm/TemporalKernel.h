//
// Created by serik1987 on 20.11.2019.
//

#ifndef MPI2_TEMPORALKERNEL_H
#define MPI2_TEMPORALKERNEL_H

#include "StimulusSaturation.h"
#include "../../../processors/Processor.h"

namespace equ {

    /**
     * A base class for any temporal kernel. The class is used to hold any temporal kernel parameter
     */
    class TemporalKernel: virtual public Processor {

    public:
        explicit TemporalKernel(mpi::Communicator& comm): Processor(comm){}

        /**
         *
         * @return the stimulus saturation
         */
        [[nodiscard]] StimulusSaturation* getStimulusSaturation(){
            StimulusSaturation* result = dynamic_cast<StimulusSaturation*>(*inputProcessorBegin());
            return result;
        }

        /**
         * Sets an appropriate stimulus saturation. The stimulus saturation is treated as the only input processor
         * to the temporal kernel
         *
         * @param saturation pointer to the stimulus saturation
         */
        void setStimulusSaturation(StimulusSaturation* saturation){
            StimulusSaturation* old_saturation = dynamic_cast<StimulusSaturation*>(*inputProcessorBegin());
            if (old_saturation != nullptr){
                removeInputProcessor(old_saturation);
            }
            addInputProcessor(saturation);
        }

        static TemporalKernel* createTemporalKernel(mpi::Communicator& comm, const std::string& mechanism_name,
                equ::Ode::SolutionParameters parameters);
    };

}


#endif //MPI2_TEMPORALKERNEL_H
