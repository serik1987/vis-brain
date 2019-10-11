#include <random>

#include "../mpi/App.h"
#include "../mpi/Datatype.h"

inline void get_datatype_info(mpi::App& app, mpi::ComplexDatatype& dtype){
    app << "Datatype extent: " << dtype.getExtent() << "\n";
    app << "Datatype lower bound: " << dtype.getLowerBound() << "\n";
    app << "Datatype size: " << dtype.getSize() << "\n";
    app << "Datatype upper bound: " << dtype.getUpperBound() << "\n";
}

struct Particle{
    double x, y, z;
    double mass;
};

const int MAXPARTICLES = 200;

void init_particles(Particle particles[], int start = 0, int finish = MAXPARTICLES){
    std::default_random_engine generator;
    std::normal_distribution<double> nd1(0.0, 50.0);

    for (int i=start; i < finish; i++){
        particles[i].x = nd1(generator);
        particles[i].y = nd1(generator);
        particles[i].z = nd1(generator);
        particles[i].mass = 1e-19;
    }
}

struct demo{
    int a;
    double b;
    int c;
};

int main(int argc, char* argv[]){
    mpi::App app(&argc, &argv);
    mpi::Communicator& comm = app.getAppCommunicator();
    int size = comm.getProcessorNumber();
    int rank = comm.getRank();
    mpi::ContiguiousDatatype TParticle(MPI_DOUBLE, 4);

    /* Initialization of particles */
    Particle particles[MAXPARTICLES];
    Particle* local_particles = new Particle[MAXPARTICLES / size];
    init_particles(local_particles, 0, MAXPARTICLES / size);
    if (size == 1){
        app << "Single process application don't work\n";
        return 1;
    } else {
        // comm.gather(local_particles, MAXPARTICLES / size, TParticle, particles, MAXPARTICLES / size, TParticle, 0);
        comm.allgather(local_particles, MAXPARTICLES/size, TParticle, particles, MAXPARTICLES/size, TParticle);
    }

    /* Printing all particles */
    if (rank == 2){
        for (int i = 0; i < MAXPARTICLES; i++){
            app << "Particle " << i << ": x = " << particles[i].x
                << " y = " << particles[i].y << " z = " << particles[i].z
                << " mass = " << particles[i].mass << "\n";
        }
    }

    app << sizeof(demo) << "\n";

    demo test;
    mpi::StructureDatatype<demo> stype(3);
    stype.addField(test.a);
    stype.addField(test.b);
    stype.addField(test.c);
    stype.commit();
    get_datatype_info(app, stype);

    delete [] local_particles;
}