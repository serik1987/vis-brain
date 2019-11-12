//
// Created by serik1987 on 11.11.2019.
//

#ifndef MPI2_PROCESSOR_H
#define MPI2_PROCESSOR_H


#include "../param/Loadable.h"
#include "../mpi/Communicator.h"
#include "../data/Matrix.h"
#include "exceptions.h"

namespace equ {

    /**
     * A base class for all equations engaged into the system
     */
    class Processor: public param::Loadable {
    private:
        mpi::Communicator& comm;

    protected:
        const char* getObjectType() const noexcept override { return "processor"; }
        data::Matrix* output = nullptr;

        /**
         *
         * @return true if the output matrix is contiguous
         */
        virtual bool isOutputContiguous() = 0;

        /**
         * Finalizes the processor
         *
         * @param destruct false except you start the method from a child destructor
         */
        virtual void finalizeProcessor(bool destruct = false) noexcept = 0;

        /**
         *
         * @return immediate name of the processor
         */
        virtual std::string getProcessorName() = 0;

    public:
        explicit Processor(mpi::Communicator& c): comm(c) {};

        Processor(const Processor& other) = delete;
        Processor& operator=(const Processor& other) = delete;
        Processor& operator=(Processor&& other) = delete;

        ~Processor() override {
            finalize(true);
        }

        /**
         *
         * @return communicator responsible for the processor job
         */
        mpi::Communicator& getCommunicator() { return comm; }

        /**
         * Initializes the processor. Initialization implies allocation of all its internal buffers and child objects
         * (mainly, matrices) filling them with a predefined values
         * This is a collective routine. It shall be run by all processes containing in the communicator simultaneously
         */
        virtual void initialize()=0;

        /**
         * This method shall be run after finishing of the simulation before launching of the next simulation.
         * it automatically starts on the destruction. This method delete all buffers and child objects.
         * This is a collective routine. It shall be run by all processes containing in the communicator
         * simultaneously
         *
         * @param destruct false except when you start this method from the destructor
         */
        void finalize(bool destruct = false) noexcept;

        /**
         * First, destroys the processor, next initializes this again
         * The method shall be run every time when the processor parameters has changed or the simulation
         * is restarted during new job
         */
        void reset(){
            finalize();
            initialize();
        }

        /**
         * Runs at each iteration
         *
         * @param time current timestamp in milliseconds
         */
        virtual void update(double time) = 0;

        /**
         *
         * @return output results from the processor
         */
        virtual data::Matrix& getOutput(){
            if (output == nullptr){
                throw uninitialized_processor();
            }
            return *output;
        }

        /**
         *
         * @return name and status of the processor
         */
        std::string getName(){
            std::stringstream ss;
            ss << getProcessorName();
            if (output != nullptr){
                ss << "  [ INITIALIZED ]";
            }
            return ss.str();
        }
    };

}


#endif //MPI2_PROCESSOR_H
