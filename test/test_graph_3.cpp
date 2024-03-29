#include <algorithm>
#include <cmath>

#include "../mpi/App.h"
#include "../mpi/AbstractGraphItem.h"
#include "../mpi/Graph.h"
#include "../mpi/GraphItem.h"
#include "../mpi/GraphCommunicator.h"
#include "../sys/Semaphore.h"

void print_rank_info(mpi::AbstractGraphItem& info){
    mpi::Communicator& comm = mpi::App::getInstance()->getAppCommunicator();
    mpi::App& app = *mpi::App::getInstance();
    app << "Total number of processes attached: " << info.getProcessorNumber() << "\n";
    app << "Is process attached: " << info.hasRank(comm.getRank()) << "\n";
}

int main(int argc, char* argv[]){
    mpi::App app(&argc, &argv);
    mpi::Communicator& comm = app.getAppCommunicator();
    int rank = comm.getRank();

    mpi::Graph brain("brain", comm);

    mpi::Graph& visual = brain.createChild("visual");
    mpi::Graph& auditory = brain.createChild("auditory");
    mpi::Graph& taste = brain.createChild("taste");

    brain.connect("visual", "auditory");
    brain.connect("auditory", "visual");
    brain.connect("visual", "taste");
    brain.connect("auditory", "taste");
    brain.connect("taste", "auditory");

    visual.assignRank(0);
    auditory.assignRank(1);
    taste.assignRank(2);

    brain.iterate([](int rank, mpi::AbstractGraphItem* g){
        mpi::Graph* g_loc = (mpi::Graph*)g;
        mpi::GraphItem& item1 = g_loc->createItem("primary");
        item1.assignRank(rank);
        mpi::GraphItem& item2 = g_loc->createItem("secondary");
        item2.assignRank(rank);
        g_loc->connect("primary", "secondary", 10);
    });

    int max_rank = rank-3;

    brain.setSource("auditory.secondary", visual);
    brain.setSource("visual.primary", auditory);
    brain.setSource("taste.secondary", auditory);
    brain.setSource("taste.primary", visual);
    brain.setSource("auditory.primary", taste);

    if (rank == 0){
        brain.connect("visual.secondary", "auditory.secondary");
        brain.connect("visual.primary", "taste.primary");
    } else if (rank == 1){
        brain.connect("auditory.primary", "taste.secondary");
        brain.connect("auditory.primary", "visual.primary");
    } else if (rank == 2){
        brain.connect("taste.secondary", "auditory.primary");
    }

    std::unordered_map<std::string, int> rankMap;
    rankMap["primary"] = 0;
    rankMap["secondary"] = 1;

    std::unordered_map<std::string, int> majorMap;
    majorMap["visual"] = 2;
    majorMap["auditory"] = 0;
    majorMap["taste"] = 4;

    for (auto it: brain){
        mpi::AbstractGraphItem* item = &(*it);
        mpi::GraphItem* node = (mpi::GraphItem*)item;
        int major = majorMap[node->getParent()->getName()];
        int minor = rankMap[node->getName()];
        node->assignRank(major + minor);
    }

    // brain.print();

    comm.barrier();

    mpi::GraphCommunicator gcomm = brain.createCommunicator();
    app.lock();
    std::cout << gcomm;
    app.unlock();

    int rate;
    int areaIndex = rank / 2;
    int layerIndex = rank % 2;
    std::string current_area;
    std::string current_layer;
    for (auto&& p = majorMap.begin(); p != majorMap.end(); ++p){
        if (p->second / 2 == areaIndex){
            current_area = p->first;
            break;
        }
    }
    for (auto&& p = rankMap.begin(); p != rankMap.end(); ++p){
        if (p->second == layerIndex){
            current_layer = p->first;
            break;
        }
    }
    if (current_area == "visual"){
        rate = 5;
    } else if (current_area == "auditory"){
        rate = 20;
    } else if (current_area == "taste"){
        rate = 200;
    } else {
        throw "unknown current layer";
    }

    comm.barrier();
    app.lock();
    app << "Current area: " << current_area << "\n";
    app << "Current layer: " << current_layer << "\n";
    app << "Rate: " << rate << "\n";
    app << "\n";
    app.unlock();

    int received_rates[100];
    gcomm.neighborAllGather(&rate, 1, MPI_INT, received_rates, 1, MPI_INT);
    comm.barrier();
    app.lock();
    for (int i=0; i < gcomm.getSourceNumber(); i++){
        app << received_rates[i] << " ";
    }
    app << "\n";
    app.unlock();

    double sendweights[100];
    for (int i=0; i < gcomm.getDestinationNumber(); i++){
        int drank = gcomm.getDestinationList()[i];
        int dareaIndex = drank / 2;
        if (areaIndex == dareaIndex){
            sendweights[i] = 200.0;
        }
        if (areaIndex == 1 && dareaIndex == 0){
            sendweights[i] = 20.0;
        }
        if (areaIndex == 1 && dareaIndex == 2){
            sendweights[i] = 0.05;
        }
        if (areaIndex == 0 && dareaIndex == 1){
            sendweights[i] = 20.0;
        }
        if (areaIndex == 0 && dareaIndex == 2){
            sendweights[i] = 0.5;
        }
        if (areaIndex == 2 && dareaIndex == 0){
            sendweights[i] = 2000.0;
        }
    }

    comm.barrier();
    app.lock();
    app << "Sending weights: ";
    for (int i=0; i < gcomm.getDestinationNumber(); i++){
        app << sendweights[i] << " ";
    }
    app << "\n";
    app.unlock();

    double recweights[100];
    gcomm.neighborAllToAll(sendweights, 1, MPI_DOUBLE, recweights, 1, MPI_DOUBLE);

    comm.barrier();
    app.lock();
    app << "Receiving weights: ";
    for (int i=0; i < gcomm.getSourceNumber(); i++){
        app << recweights[i] << " ";
    }
    app << "\n";
    app.unlock();

}