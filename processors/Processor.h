//
// Created by serik1987 on 11.11.2019.
//

#ifndef MPI2_PROCESSOR_H
#define MPI2_PROCESSOR_H

#include <list>

#include "../param/Loadable.h"
#include "../mpi/Communicator.h"
#include "../data/Matrix.h"
#include "exceptions.h"
#include "Ode.h"

namespace equ {

    /**
     * A base class for all equations engaged into the system
     */
    class Processor: public param::Loadable {
    private:
        mpi::Communicator& comm;
        Processor* outputProcessor = nullptr;
        std::list<Processor*> inputProcessors;

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
         * The method shall destroy all matrices and buffers except output matrix which will be destroyed by
         * the processor routine
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
         * Runs at each iteration.
         * for stimuli: changes the stimulus state and prepares output matrix at a certain time
         * for equations: just prepares the output matrix, without changing the state
         * for ODEs: makes all routines after changing the main output matrix.
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

        /**
         *
         * @return a processor for which current processor is treated as input processor
         */
        Processor* getOutputProcessor() { return outputProcessor; }

        /**
         * Adds input processors at the end of the list
         *
         * @param other reference to the input processor
         */
        void addInputProcessor(Processor* pother);

        /**
         * Removes the input processor from the input processor list
         *
         * @param pother list of the other processors
         */
        void removeInputProcessor(Processor* pother);

        /**
         *
         * @return iterator to the first processor within the list of the input processors
         */
        std::list<Processor*>::iterator inputProcessorBegin() { return inputProcessors.begin(); }

        /**
         *
         * @return iterator to the end of the list of the input processors
         */
        std::list<Processor*>::iterator inputProcessorEnd() { return inputProcessors.end(); }

        /**
         *
         * @return total number of the input processors
         */
        int getInputProcessorNumber(){
            return inputProcessors.size();
        }

        /**
         * Returns a certain input processor
         *
         * @param index number of the input processor
         * @return pointer to the input processor
         */
        Processor* getInputProcessor(int index){
            auto it = inputProcessorBegin();
            for (int i=0; i < index; ++i){
                ++it;
            }
            return *it;
        }

        /**
         * Recursively prints all other processes within the iterator. the processes will be printed to cout
         *
         * @param level shall be zero when the function runs outside the class
         * @param root rank of the process within the processor's communicator that will print the info
         */
        void printAllProcessors(int level = 0, int root = 0);

        /**
         * Creates new processor
         *
         * @param comm communicator for which the processor has to be created
         * @param mechanism a mechanism
         * @param parameters an object generated by the solution method
         * @return pointer to the processor
         */
        static Processor* createProcessor(mpi::Communicator& comm, const std::string& mechanism,
                equ::Ode::SolutionParameters parameters = {});

        /**
         * Looks whether external mechanisms exists as a real .so file
         *
         * @param category mechanism category (name of a folder where an appropriate .so file shall be looked).
         * @param external_name minor name of the mechanism
         * @return Absolute or relative path to the .so file.
         * @throws param::UnknownMechanism if the function is failed to find an appropriate .so file
         */
        static std::string lookExternalMechanism(const std::string& category, const std::string& external_name);
    };

}


#endif //MPI2_PROCESSOR_H
