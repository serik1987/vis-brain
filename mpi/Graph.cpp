//
// Created by User on 10.07.2019.
//

#include "Graph.h"
#include "GraphItem.h"
#include "App.h"

#define MAX_SIZE 1024

namespace mpi{

    Graph::Graph(const std::string &name, mpi::Communicator &c): AbstractGraphItem(name), comm(&c) {

    }

    Graph::Graph(const Graph& other){
        copy(other);
    }

    Graph& Graph::operator=(const Graph& other){
        copy(other);
        return *this;
    }

    void Graph::copy(const mpi::Graph &other) {
        AbstractGraphItem::copy(other);
        for (auto& it: other.children){
            AbstractGraphItem* old_item = it.second;
            AbstractGraphItem* new_item;
            // std::cout << "CREATING NEW OBJECTS\n";
            if (old_item->isContainer()){
                Graph* old_graph = (Graph*)old_item;
                Graph* new_graph = new Graph(*old_graph);
                new_item = new_graph;
            } else {
                GraphItem* old_it = (GraphItem*)old_item;
                GraphItem* new_it = new GraphItem(*old_it);
                new_item = new_it;
            }
            addChild(new_item);
        }
    }

    Graph::~Graph(){
        for (auto& it: children){
            delete it.second;
            // std::cout << "THE ITEM HAS BEEN DELETED\n";
        }
    }

    AbstractGraphItem& Graph::getChild(const std::string &fullname) {
        using std::string;
        string childName;
        string tail;

        size_t pos = fullname.find(".");
        if (pos == string::npos){
            childName = fullname;
            tail = "";
        } else {
            childName = fullname.substr(0, pos);
            tail = fullname.substr(pos+1);
        }

        if (children.find(childName) == children.end()){
            throw address_not_found();
        }
        AbstractGraphItem* item = children[childName];

        if (tail == "") {
            return *item;
        } else {
            Graph* child = dynamic_cast<Graph*>(item);
            if (child == nullptr){
                throw address_not_found();
            }
            return child->getChild(tail);
        }
    }

    void Graph::print(int level) {
        App& app = *App::getInstance();
        for (auto it = begin(level); it != end(level); ++it){
            app << it.getName() << " ";
            if ((*it)->isContainer()){
                app << "[CONTAINER] ";
            } else {
                GraphItem* gi = (GraphItem*)(*it);
                if (gi->isGhost()){
                    app << "[GHOST] ";
                }
            }
            app << "NUMPROCS = " << (*it)->getProcessorNumber() << " ";
            if ((*it)->hasConnections()){
                for (auto cit = (*it)->connBegin(); cit != (*it)->connEnd(); ++cit){
                    app << (*cit)->getFullName() << " ";
                }
            } else {
                app << "NO CONNECTIONS";
            }
            app << "\n";
        }
    }

    void Graph::connect(const std::string &source_name, const std::string &target_name, int weight) {
        AbstractGraphItem& source = getChild(source_name);
        AbstractGraphItem& target = getChild(target_name);
        source.addDestination(&target, weight);
    }

    void Graph::setSource(const std::string &source, mpi::Graph &container) {
        comm->barrier();
        try {
            mpi::AbstractGraphItem &item = getChild(source);
            if (item.getProcessorNumber() > 0){
                int sourceRank = *item.rankBegin();
                if (comm->getRank() == sourceRank){
                    if (container.getProcessorNumber() > 0){
                        for (auto rnk_it = container.rankBegin(); rnk_it != container.rankEnd(); ++rnk_it){
                            int destRank = *rnk_it;
                            if (!item.hasRank(destRank)){
                                comm->send(source.c_str(), (int)source.size()+1, MPI_CHAR, destRank, 0);
                            }
                        }
                    }
                }
            }
        } catch (address_not_found&) {

        }
        for (auto rnk_it = container.rankBegin(); rnk_it != container.rankEnd(); ++rnk_it){
            int rank = *rnk_it;
            if (rank == comm->getRank()){
                char buffer[MAX_SIZE];
                MPI_Status status = comm->recv(buffer, MAX_SIZE, MPI_CHAR, MPI_ANY_SOURCE, 0);
                createNode(buffer, rank);
            }
        }
        comm->barrier();
    }

    void Graph::createNode(const std::string &address, int rank) {
        size_t pos = address.find(".");
        std::string childName;
        std::string tail;
        if (pos != std::string::npos){
            childName = address.substr(0, pos);
            tail = address.substr(pos+1);
        } else {
            childName = address;
            tail = "";
        }
        if (tail == ""){
            if (!exists(childName)){
                GraphItem& node = createItem(childName);
                node.assignRank(rank);
                node.setGhost();
            }
        } else {
            mpi::Graph* container;
            if (!exists(childName)){
                container = &createChild(childName);
                container->assignRank(rank);
            } else {
                AbstractGraphItem* item = children[childName];
                container = dynamic_cast<mpi::Graph*>(item);
                if (container == nullptr){
                    throw address_not_found();
                }
            }
            container->createNode(tail, rank);
        }
    }

    GraphCommunicator Graph::createCommunicator(bool reorder, MPI_Info info) {
        checkComm();

        std::vector<int> sources(0);
        std::vector<int> degrees(0);
        std::vector<int> destinations(0);
        std::vector<int> weights(0);

        for (auto it: *this){
            GraphItem* node = (GraphItem*)&(*it);
            if (!node->isGhost()){
                int rank = *node->rankBegin();
                int deg_number = 0;
                int idx = 0;
                for (auto it2 = node->connBegin(); it2 != node->connEnd(); ++it2){
                    int dest = *((*it2)->rankBegin());
                    destinations.push_back(dest);
                    weights.push_back(node->getWeights()[idx]);
                    idx++;
                    deg_number++;
                }
                sources.push_back(rank);
                degrees.push_back(deg_number);
            }
        }
        return GraphCommunicator(*comm, (int)sources.size(), sources.data(), degrees.data(),
                destinations.data(), weights.data(), reorder, info);
    }

    inline void Graph::checkComm() {
        std::unordered_set<int> ranks;
        for (auto it: *this){
            AbstractGraphItem& item = *it;
            GraphItem* node = (GraphItem*)&item;
            if (!node->isGhost()) {
                if (item.getProcessorNumber() != 1) {
                    throw not_graph_comm();
                }
                int rank = *item.rankBegin();
                if (rank < 0 || rank >= comm->getProcessorNumber()){
                    throw not_graph_comm();
                }
                if (ranks.find(rank) != ranks.end()) {
                    throw not_graph_comm();
                }
                ranks.insert(rank);
            }
        }
        int size = (int)ranks.size();
        int total_size;
        comm->allReduce(&size, &total_size, 1, MPI_INT, MPI_SUM);
        if (total_size != comm->getProcessorNumber()){
            throw not_graph_comm();
        }
    }


    /* Implementation of the iterator methods */
    void mpi::Graph::Iterator::computeChild() {
        bool done = false;
        while (!done){
            if (internalIterator == parent->children.end()){
                done = true;
                childIterator = nullptr;
                reference = nullptr; // reference is none only in case of NULL iterator
                name = "";
            } else {
                AbstractGraphItem *item = internalIterator->second;
                if (!item->isContainer() || level == 0){
                    childIterator = nullptr;
                    reference = item;
                    name = parent->getName() + "." + reference->getName();
                    done = true;
                } else {
                    Graph* graph = (Graph*)item;
                    if (graph->children.size() == 0){
                        ++internalIterator;
                    } else {
                        Iterator x = graph->begin(level-1);
                        if (x.reference == nullptr){ // the reference is assigned to zero for end iterators only
                            ++internalIterator;
                        } else {
                            childIterator = new Iterator(x);
                            reference = x.reference;
                            name = parent->getName() + "." + x.name;
                            done = true;
                        }
                    }
                }
            }
        }
    }

    void mpi::Graph::Iterator::increment() {
        if (childIterator != nullptr){
            ++(*childIterator);
            if (childIterator->reference == nullptr){
                delete childIterator;
                ++internalIterator;
                computeChild();
            } else {
                reference = childIterator->reference;
                name = parent->getName() + "." + childIterator->name;
            }
        } else {
            if (reference == nullptr){
                throw graph_iteration_error();
            } else {
                ++internalIterator;
                computeChild();
            }
        }
    }

    Graph::Iterator::Iterator(const Graph::Iterator &other) {
        level = other.level;
        parent = other.parent;
        internalIterator = other.internalIterator;
        if (other.childIterator != nullptr){
            childIterator = new Iterator(*other.childIterator);
        } else {
            childIterator = nullptr;
        }
        reference = other.reference;
        name = other.name;
    }

}
