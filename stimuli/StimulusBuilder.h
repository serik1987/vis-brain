//
// Created by serik1987 on 30.11.2019.
//

#ifndef MPI2_STIMULUSBUILDER_H
#define MPI2_STIMULUSBUILDER_H

#include "Stimulus.h"
#include "../param/Loadable.h"

namespace stim {

    /**
     * This is an auxiliary class that is used to apply the code embedded to
     * param::Loadable::loadParameters and param::Loadable::broadcastParameters
     * for effective and error-none loading and broadcasting the stimulus parameters
     * without duplicating the code already written and debugging the duplicated code
     *
     * How to use the builder.
     * 1) Create the builder
     * stim::StimulusBuilder builder(comm)
     *
     * 2) load/broadcast parameters
     * builder.loadParameters(obj) // shall be run by the process with rank = 0 within the
     * Application communicator
     * builder.brodcastParameters()
     * these method create new stimulus, load and broadcast
     * its parameters
     *
     * 3) Retrieve new stimulus
     * stim::Stimulus* stimulus = builder.getStimulus()
     *
     * Builder will be destroyed automatically when this will be beyond the scope of the
     *
     */
    class StimulusBuilder: public param::Loadable {
    private:
        mpi::Communicator& comm;
        Stimulus* stimulus = nullptr;
        std::string mechanism;

    protected:
        [[nodiscard]] const char* getObjectType() const noexcept override { return "processor"; };
        void loadParameterList(const param::Object& source) override;
        void broadcastParameterList() override;
        void setParameter(const std::string& name, const void* pvalue) override {
            throw param::IncorrectParameterName(name, "stimulus builder");
        }

    public:
        explicit StimulusBuilder(mpi::Communicator& c): comm(c) {};

        [[nodiscard]] Stimulus* getStimulus() { return stimulus; }
    };

}


#endif //MPI2_STIMULUSBUILDER_H
