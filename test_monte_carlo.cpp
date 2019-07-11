#include <iostream>
#include <initializer_list>
#include <vector>
#include <algorithm>

#include "mpi/App.h"
#include "mpi/Group.h"

#define REQUEST 1
#define REPLY 2
#define CHUNKSIZE 1000

int main(int argc, char* argv[]) {

    /* MPI initialization */
    mpi::App app(&argc, &argv);
    mpi::Communicator& world = app.getAppCommunicator();
    int numprocs = world.getProcessorNumber();
    int myid = world.getRank();
    int server = numprocs - 1;
    if (numprocs < 2){
        std::cerr << "[FATAL ERROR] Too little processes were chosen. The application can no longer work\n";
        exit(EXIT_FAILURE);
    }
    mpi::Communicator workers = world.exclude(server);


    // Input data
    double epsilon;
    app >> epsilon;

    // Variable declaration
    int request;
    int in, out, done;
    int max = RAND_MAX;
    int iter;
    int rands[CHUNKSIZE];
    int totalin, totalout;
    double pi = 0;
    double old_pi = 0;

    // Working code for the server
    if (!workers){
        do{
            MPI_Status status;
            world.recv(&request, 1, MPI_INT, MPI_ANY_SOURCE, REQUEST, &status);
            if (request){
                for (int i=0; i < CHUNKSIZE; ){
                    rands[i] = std::rand();
                    if (rands[i] <= max) i++;
                }
                world.send(rands, CHUNKSIZE, MPI_INT, status.MPI_SOURCE, REPLY);
            }
        } while (request > 0);
    }

    if (workers) {
        // Working code for the client
        request = 1;
        done = in = out = 0;
        world.send(&request, 1, MPI_INT, server, REQUEST);
        int workerid = workers.getRank();
        iter = 0;
        while (!done){
            iter++;
            request = 1;
            world.recv(rands, CHUNKSIZE, MPI_INT, server, REPLY);
            for (int i=0; i < CHUNKSIZE; ) {
                double x = (((double)rands[i++])/max)*2-1;
                double y = (((double)rands[i++])/max)*2-1;
                if (x*x + y*y < 1){
                    in++;
                } else {
                    out++;
                }
            }

            workers.allreduce(&in, &totalin, 1, MPI_INT, MPI_SUM);
            workers.allreduce(&out, &totalout, 1, MPI_INT, MPI_SUM);

            old_pi = pi;
            pi = (4.0*totalin) / (totalin + totalout);
            double error = fabs(pi - old_pi);
            if (workers.getRank() == 0){
                app << "Iteration number: " << iter << "\n";
                app << "Old value: " << old_pi << "\n";
                app << "New value: " << pi << "\n";
                app << "Error: " << error << "\n";
            }
            done = (error < epsilon || iter > 1000000);
            if (done){
                request = 0;
            } else {
                request = 1;
            }
            world.send(&request, 1, MPI_INT, server, REQUEST);
        }
    }

    return 0;
}