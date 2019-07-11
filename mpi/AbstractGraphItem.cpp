//
// Created by User on 10.07.2019.
//

#include "AbstractGraphItem.h"
#include "Graph.h"

namespace mpi{

    AbstractGraphItem::AbstractGraphItem(const std::string &item_name):
        name(item_name){
        if (name.find(".") != std::string::npos){
            throw node_name_error();
        }
    }

    void AbstractGraphItem::copy(const mpi::AbstractGraphItem &other) {
        name = other.name;
        weights = other.weights;
        ranks = other.ranks;
    }

    std::string AbstractGraphItem::getFullName() {
        std::string fullName;
        if (parent == nullptr){
            fullName = getName();
        } else {
            fullName = parent->getFullName() + "." + getName();
        }
        return fullName;
    }


}