//
// Created by User on 10.07.2019.
//

#ifndef MPI2_GRAPH_H
#define MPI2_GRAPH_H

#include <unordered_map>
#include <string>

#include "AbstractGraphItem.h"
#include "GraphItem.h"
#include "GraphCommunicator.h"

namespace mpi {

    class GraphItem;

    class Graph: public AbstractGraphItem {
    private:
        std::unordered_map<std::string, AbstractGraphItem*> children;
        mpi::Communicator* comm;

        void checkComm();

    protected:
        void copy(const Graph& other);

    public:
        Graph(): AbstractGraphItem{} {};
        Graph(const std::string& name, mpi::Communicator& comm);
        Graph(const Graph& graph);
        ~Graph();

        Graph& operator=(const Graph& other);

        /**
         *
         * @return true if the graph node can contain child nodes, false otherwise
         */
        bool isContainer() override { return true; }

        bool exists(const std::string& name){
            return children.find(name) != children.end();
        }

        /**
         * Creates the child graph
         *
         * @param name of the child graph
         * @return alias for the child graph. Please, bear in mind that the child node doesn't coincide to its copy
         */
        Graph& createChild(const std::string& name){
            // std::cout << "GRAPH CREATED\n";
            Graph* child = new Graph(name, *comm);
            children[name] = child;
            child->setParent(this);
            child->comm = comm;
            return *child;
        }

        GraphItem& createItem(const std::string& name){
            GraphItem* child = new GraphItem(name);
            children[name] = child;
            child->setParent(this);
            return *child;
        }




        /**
         * Returns the node
         *
         * @param fullname the full path to the node.The path is formed by mentioning all leaves within the path
         * separated by dot.
         * @return the node
         */
        AbstractGraphItem& getChild(const std::string& fullname);

        /**
         * Adds the item to the child
         *
         * @param pointer to the item to add
         */
        void addChild(AbstractGraphItem* item){
            const std::string& name = item->getName();
            children[name] = item;
            item->setParent(this);
        }

        /**
         * Launches the function over all immediate children. The function will be launched only for those
         * processes which ranks are within the assigned rank and which communicators belong to the graph communicator
         *
         * @tparam Function the function like void f(int rank, mpi::AbstractGraphItem& item)
         * @param f Instance of that function
         */
        template <typename Function> void iterate(Function f){
            for (auto& it: children){
                if (it.second->hasRank(comm->getRank())){
                    f(comm->getRank(), it.second);
                }
            }
        }


        /**
         * Prints all items in the graph
         */
        void print(int level = INT_MAX);

        /**
         * Connects two nodes of the graph
         *
         * @param source relative path to the source node
         * @param target relative path to the second node
         * @param weight connection weight. This is the estimate of the total connection load
         */
        void connect(const std::string& source, const std::string& target, int weight = 1);

        /**
         * Sets the source for the specified container. The function has no effect when source and container
         * are served by the same processes but has effect if the serving processes are different.
         *
         * The function will transmit the node which full path is defined as source to the destination container
         * when and only when:
         * (a) More than one processes are assigned to the rank
         * (b) All processes attached to the node call this function (one of them only will send this message)
         * (c) At least one process is attached to the destination container
         * (d) at least one process attached to the container has rank that in not in the source process
         *
         * @param source the full path to the node that has already been created
         * @param container address of the node in the address space of the other process
         */
        void setSource(const std::string& source, mpi::Graph& container);

        /**
         * Creates node with the specified address. The node will be created only when it doesn't exist
         * all containers will be created automatically
         *
         * @param address the address of the node to create
         * @param rank rank of the process that will be assigned to the node and all creating containers
         */
        void createNode(const std::string& address, int rank);


        /**
         * Creates the graph communicator
         *
         * @param reorder (optional, default is true) true if change ranks of the source processes, false otherwise
         * @param info the MPI_Info structure that contains hints for the implementation
         * @return mpi::GraphCommunicator instance
         */
        GraphCommunicator createCommunicator(bool reorder = true, MPI_Info info = MPI_INFO_NULL);



        /**
         * The internal iterator for use in for loops
         */
        class Iterator{
        private:
            int level;
            Graph* parent;
            std::unordered_map<std::string, AbstractGraphItem*>::iterator internalIterator;
            Iterator* childIterator;
            bool containerIteration;
            std::string name;
            AbstractGraphItem* reference;

            /**
             * Uses the fields parent, internalIterator in order to compute childIterator, containerInteration,
             * name and reference
             */
            void computeChild();

            /**
             * Moves the iterator to one item
             */
            void increment();
        public:
            Iterator(Graph* par, std::unordered_map<std::string, AbstractGraphItem*>::iterator it,
                    int lev = INT_MAX): parent(par), level(lev), internalIterator(it) {
                computeChild();
            };

            Iterator(const Iterator& other);

            ~Iterator(){
                if (childIterator != nullptr) {
                    delete childIterator;
                }
            }

            /**
             *
             * @return the current iteration level
             */
            int getLevel() { return level; }

            /**
             *
             * @return the parent of the current graph node
             */
            Graph* getParent() { return parent; }

            /**
             *
             * @return the child iterator (that is, iterator over child container
             */
            Iterator* getChildIterator() { return childIterator; }

            /**
             * Link to the current item
             *
             * @return
             */
            AbstractGraphItem* operator*(){
                return reference;
            }

            Iterator& operator++(){
                increment();
                return *this;
            }

            Iterator operator++(int){
                Iterator work = *this;
                increment();
                return work;
            }

            bool operator==(const Iterator& other){
                return parent == other.parent && reference == other.reference;
            }

            bool operator!=(const Iterator& other){
                return parent != other.parent || reference != other.reference;
            }

            /**
             *
             * @return the full name of the iterated item
             */
            const std::string& getName(){
                return name;
            }
        };

        Iterator begin(int level = INT_MAX){
            return Iterator(this, children.begin(), level);
        }

        Iterator end(int level = INT_MAX){
            return Iterator(this, children.end(), level);
        }

    };

}


#endif //MPI2_GRAPH_H
