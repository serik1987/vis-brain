//
// Created by serik1987 on 04.11.2019.
//

#include "NoiseEngine.h"
#if DEBUG==1
#include "../../log/output.h"
#endif

namespace data::noise{

#if DEBUG==1
    template class NoiseEngineTemplate<DebugNoiseGenerator, SECONDARY_GENERATOR>;
#endif
    template class NoiseEngineTemplate<PRIMARY_GENERATOR, SECONDARY_GENERATOR>;

    template<typename PrimaryGenerator, typename SecondaryGenerator>
    NoiseEngineTemplate<PrimaryGenerator, SecondaryGenerator>::NoiseEngineTemplate() {
        mpi::Communicator& comm = Application::getInstance().getAppCommunicator();
        int seed;
        if (comm.getRank() == 0) {
            primaryGenerator = new PrimaryGenerator();
            seed = (*primaryGenerator)();
        }
        comm.broadcast(&seed, 1, MPI_INT, 0);
        secondaryGenerator = new SecondaryGenerator(seed);
        if (comm.getRank() != 0){
            secondaryGenerator->discard(comm.getRank());
        }
    }

    template<typename PrimaryGenerator, typename SecondaryGenerator>
    NoiseEngineTemplate<PrimaryGenerator, SecondaryGenerator>::~NoiseEngineTemplate(){
        delete primaryGenerator;
        delete secondaryGenerator;
    }

    template<typename PrimaryGenerator, typename SecondaryGenerator>
    unsigned long int NoiseEngineTemplate<PrimaryGenerator, SecondaryGenerator>::operator()(){
        unsigned long int random_number = (*secondaryGenerator)();
        int numprocs = Application::getInstance().getAppCommunicator().getProcessorNumber();
        secondaryGenerator->discard(numprocs-1);
        return random_number;
    }

    template <typename PrimaryGenerator, typename SecondaryGenerator>
    void NoiseEngineTemplate<PrimaryGenerator, SecondaryGenerator>::seed(){
        unsigned int seed = (*primaryGenerator)();
        secondaryGenerator->seed(seed);
    }

    template<typename PrimaryGenerator, typename SecondaryGenerator>
    void NoiseEngineTemplate<PrimaryGenerator, SecondaryGenerator>::discard(unsigned long long z) {
        int numprocs = Application::getInstance().getAppCommunicator().getProcessorNumber();
        unsigned long long true_z = z * numprocs;
        secondaryGenerator->discard(true_z);
    }
}