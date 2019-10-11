#include "../mpi/App.h"

class Sequence{
private:

    mpi::Communicator& c;
    mpi::Communicator lc;
    int prevRank, nextRank;

    static int seqKeyVal;

public:
    Sequence(mpi::Communicator& comm):
        c(comm), lc(comm.duplicate()){
        int rank = comm.getRank();
        prevRank = rank-1;
        if (prevRank < 0) prevRank = -1;
        nextRank = rank+1;
        if (nextRank >= comm.getProcessorNumber()) nextRank = -1;
    }

    void begin(mpi::Communicator& comm){
        if (prevRank != -1){
            lc.recv(nullptr, 0, MPI_DATATYPE_NULL, prevRank, 0);
        }
    }

    void end(mpi::Communicator& comm){
        if (nextRank != -1){
            lc.send(nullptr, 0, MPI_DATATYPE_NULL, nextRank, 0);
        }
    }
};

int main(int argc, char* argv[]){
    mpi::App app(&argc, &argv);
    mpi::Communicator& comm0 = app.getAppCommunicator();

    Sequence seq(comm0);
    seq.begin(comm0);
    app << "My rank is " << comm0.getRank() << "\n";
    seq.end(comm0);
}