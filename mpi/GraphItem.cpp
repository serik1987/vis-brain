//
// Created by User on 10.07.2019.
//

#include "GraphItem.h"

namespace mpi{

    void GraphItem::copy(const mpi::GraphItem &other) {
        AbstractGraphItem::copy(other);
        ghost = other.ghost;
    }

}
