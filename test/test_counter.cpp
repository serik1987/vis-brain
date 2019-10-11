#include "../mpi/App.h"

#ifdef _WIN32
    #include <windows.h>
    inline void sleep(unsigned int seconds){
        Sleep(seconds * 1000);
    }
#else
    #ifdef __unix__
    #include <unistd.h>
    #endif
#endif

namespace mpe{

    class Counter{
    private:
        mpi::Communicator dataComm;
        mpi::Communicator counterComm;
    public:
        mpi::Communicator& getDataComm() { return dataComm; }
        mpi::Communicator& getCounterComm() { return counterComm; }

        enum Tags{REQUEST, VALUE, GOAWAY};

        Counter(mpi::Communicator& comm): dataComm(MPI_COMM_NULL), counterComm(MPI_COMM_NULL){
            int counter = 0;
            int message, done = 0, myid, numprocs, server, color;
            MPI_Status status;

            numprocs = comm.getProcessorNumber();
            myid = comm.getRank();
            server = numprocs-1;
            counterComm = comm.duplicate();
            if (myid == server){
                color = MPI_UNDEFINED;
            } else {
                color = 0;
            }
            dataComm = comm.split(color, myid);

            if (myid == server){
                while (!done){
                    status = counterComm.recv(&message, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG);
                    if (status.MPI_TAG == REQUEST){
                        counterComm.send(&counter, 1, MPI_INT, status.MPI_SOURCE, VALUE);
                        counter++;
                    } else if (status.MPI_TAG == GOAWAY){
                        done = 1;
                    } else {
                        mpi::App* app = mpi::App::getInstance();
                        *app << "Bad tag " << status.MPI_TAG << " was sent to MPE counter\n";
                    }
                }
            }
        }

        int getServerRank(){
            return counterComm.getProcessorNumber() - 1;
        }

        ~Counter(){
            int myid, numprocs;

            myid = counterComm.getRank();
            numprocs = counterComm.getProcessorNumber();


            if (myid == 0){
                counterComm.send(NULL, 0, MPI_DATATYPE_NULL, numprocs-1, GOAWAY);
            }
        }

        int next(){
            int server, numprocs, value;
            numprocs = counterComm.getProcessorNumber();
            server = numprocs - 1;
            counterComm.send(&server, 1, MPI_INT, server, REQUEST);
            *mpi::App::getInstance() << "The message was sent\n";
            counterComm.recv(&value, 1, MPI_INT, server, VALUE);
            return value;
        }
    };

}

int main(int argc, char* argv[]){
    mpi::App app(&argc, &argv);
    mpi::Communicator& comm = app.getAppCommunicator();
    mpe::Counter counter(comm);
    if (comm.getRank() != counter.getServerRank()) {
        app << "Communicator has been launched\n";
        app << "Counter value: " << counter.next() << "\n";
    }
}