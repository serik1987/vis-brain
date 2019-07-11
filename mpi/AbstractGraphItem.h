//
// Created by User on 10.07.2019.
//

#ifndef MPI2_ABSTRACTGRAPHITEM_H
#define MPI2_ABSTRACTGRAPHITEM_H

#include <string>
#include <unordered_set>

#include "Communicator.h"

namespace mpi {

    class Communicator;
    class Graph;

    /**
     * This is an abstract class for mpi::Graph and mpi::GraphItem
     */
    class AbstractGraphItem {
    private:
        Graph* parent = nullptr;
        std::string name;

    protected:
        std::vector<AbstractGraphItem*> destinations;
        std::vector<int> weights;
        std::unordered_set<int> ranks;

        virtual void copy(const AbstractGraphItem& other);
    public:
        AbstractGraphItem(): name("unknown") {};
        AbstractGraphItem(const std::string& name);
        virtual ~AbstractGraphItem() {};

        /**
         *
         * @return name of the node
         */
        const std::string& getName() const { return name; }


        /**
         *
         * @return true if the graph node can contain child nodes, false otherwise
         */
        virtual bool isContainer()=0;

        /**
         * Sets the graph that item belongs to
         *
         * @param graph
         */
        void setParent(Graph* graph) { parent = graph; }

        /**
         *
         * @return the graph the item belongs to
         */
        Graph* getParent() { return parent; }

        /**
         * Assignes process to the node. All other assigned processes were automatically detached
         *
         * @param rank the rank assigned
         */
        void assignRank(int rank){
            ranks.clear();
            ranks.insert(rank);
        }

        /**
         * Attaches the process
         *
         * @param rank of the process to attach
         */
        void attachRank(int rank){
            ranks.insert(rank);
        }

        /**
         * Detaches the process
         *
         * @param rank of the process to detach
         */
        void detachRank(int rank){
            ranks.erase(rank);
        }

        /**
         *
         * @return number of processes attached
         */
        int getProcessorNumber(){
            return (int)ranks.size();
        }

        /**
         * Checks whether the process with a certain rank has been attached
         *
         * @param rank th check
         * @return true if the rank has been attached, false otherwise
         */
        bool hasRank(int rank){
            return ranks.find(rank) != ranks.end();
        }

        /**
         * Adds the destination to the node
         *
         * @param item the destination item
         * @param weight item weight
         */
        void addDestination(AbstractGraphItem* item, int weight = 1){
            destinations.push_back(item);
            weights.push_back(weight);
        }

        /**
         *
         * @return true if there is at least one destination node
         */
        bool hasConnections(){ return destinations.size() > 0; }

        /**
         *
         * @return iterator to the first connection
         */
        auto connBegin() { return destinations.begin(); }

        /**
         *
         * @return iterator to the second oonnection
         */
        auto connEnd() { return destinations.end(); }

        /**
         *
         * @return link to the first item
         */
        auto rankBegin() { return ranks.begin(); }


        /**
         *
         * @return link to the second item
         */
        auto rankEnd() { return ranks.end(); }

        /**
         *
         * @return returns full name of the node, for information purpose
         */
        std::string getFullName();

        /**
         * Returns an array containing connection weights
         *
         * @return
         */
        std::vector<int>& getWeights() { return weights; }
    };

    class address_not_found: public exception{
        const char* what(){
            return "Incorrect address was given in Graph::getChild(...)";
        }
    };

    class node_name_error: public exception{
        const char* what(){
            return "Name of the graph node contains dot symbol";
        }
    };

    class graph_iteration_error: public exception{
        const char* what(){
            return "End of the graph has reached. Further increment is no longer possible";
        }
    };

    class not_graph_comm: public exception{
        const char* what(){
            return "You can't create graph communicator from this graph";
        }
    };

}


#endif //MPI2_ABSTRACTGRAPHITEM_H
