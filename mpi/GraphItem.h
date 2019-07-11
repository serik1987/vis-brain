//
// Created by User on 10.07.2019.
//

#ifndef MPI2_GRAPHITEM_H
#define MPI2_GRAPHITEM_H


#include "AbstractGraphItem.h"

namespace mpi {

    class GraphItem: public AbstractGraphItem {
    private:
        bool ghost = false;

    protected:
        void copy(const GraphItem&);
    public:

        GraphItem(): AbstractGraphItem() {};
        GraphItem(const std::string& name): AbstractGraphItem(name) {};

        /**
         *
         * @return true if this item is a ghost item.
         */
        bool isGhost() { return ghost; }

        /*
         * Marks that this mode is ghost
         */
        void setGhost() { ghost = true; };

        /**
         *
         * @return true if the graph node can contain child nodes, false otherwise
         */
        bool isContainer() { return false; }
    };

}


#endif //MPI2_GRAPHITEM_H
