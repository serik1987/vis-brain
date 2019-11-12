//
// Created by User on 11.07.2019.
//

#include <cstring>
#include "App.h"

namespace mpi {
    bool App::initialized = false;
    App *App::instance = nullptr;

    void App::broadcastString(std::string &sample, int root) {
        int size = sample.size();
        mpi::Communicator& comm = getAppCommunicator();
        comm.broadcast(&size, 1, MPI_INTEGER, root);
        char* buffer = new char[size+1];
        strcpy(buffer, sample.c_str());
        comm.broadcast(buffer, size+1, MPI_CHAR, root);
        sample.assign(buffer);
        delete [] buffer;
    }

    void App::broadcastInteger(int &data, int root) {
        int buffer = data;
        getAppCommunicator().broadcast(&buffer, 1, MPI_INT, root);
        data = buffer;
    }

    void App::broadcastBoolean(bool &value, int root) {
        getAppCommunicator().broadcast(&value, 1, MPI_C_BOOL, root);
    }

    void App::broadcastDouble(double &data, int root) {
        getAppCommunicator().broadcast(&data, 1, MPI_DOUBLE, root);
    }
}

