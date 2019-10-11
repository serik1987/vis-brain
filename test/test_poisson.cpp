#include <cmath>
#include "../mpi/App.h"
#include "../mpi/CartesianCommunicator.h"

const int width = 230;
const int height = 230;
int local_height;
int start_index;
int finish_index;
int local_number;
int coord;
int bottom, top;

void compute_local_indices(int coord, int numprocs){
    local_height = (int)ceil((double)height/numprocs);
    start_index = local_height * coord;
    finish_index = start_index + local_height;
    if (coord == numprocs-1){
        finish_index = height;
        local_height = finish_index - start_index;
    }
    local_number = width * local_height;
}

inline int get_abscissa(int local_index){
    return local_index % width;
}

inline int get_ordinate(int local_index){
    return local_index / width + start_index;
}

inline int get_local_index(int x, int y){
    return (y - start_index)*width + x;
}

double point_process(int i, int j){
    if (i == width/2 && j == height/2){
        return 10.0;
    } else {
        return 0.0;
    }
}

double (*filling_function)(int, int) = point_process;

void fill (double* src, bool fill_zeros = false){
    for (int local_index = 0; local_index < local_number; local_index++) {
        int x = get_abscissa(local_index);
        int y = get_ordinate(local_index);
        if (fill_zeros){
            src[local_index] = 0.0;
        } else {
            src[local_index] = filling_function(x, y);
        }
    }
}

void print(mpi::Communicator& comm, double* data){
    double local_max = 0;
    for (int i = 0; i < local_number; i++){
        double value = std::fabs(data[i]);
        if (value > local_max) local_max = value;
    }
    double max;
    comm.allreduce(&local_max, &max, 1, MPI_DOUBLE, MPI_MAX);
    if (max == 0) max = 1.0;
    if (coord == 0){
        std::cout << "MAX = " << max << std::endl;
    } else {
        comm.recv(nullptr, 0, MPI_DATATYPE_NULL, bottom, 1000, MPI_STATUS_IGNORE);
    }
    for (int y = start_index; y < finish_index; y++){
        for (int x = 0; x < width; x++){
            int idx = get_local_index(x, y);
            switch ((int)std::floor(std::fabs(data[idx])*10/max)){
                case 0: std::cout << "!"; break;
                case 1: std::cout << "@"; break;
                case 2: std::cout << "#"; break;
                case 3: std::cout << "$"; break;
                case 4: std::cout << "%"; break;
                case 5: std::cout << "^"; break;
                case 6: std::cout << "&"; break;
                case 7: std::cout << "*"; break;
                case 8: std::cout << "("; break;
                default: std::cout << ")"; break;
            }
        }
        std::cout << std::endl;
    }
    if (coord < comm.getProcessorNumber() - 1){
        comm.send(nullptr, 0, MPI_DATATYPE_NULL, top, 1000);
    }
}

void data_exchange1(mpi::App& app, mpi::Communicator& comm, double* matrix){
    double* low_ghost = matrix - width;
    double* high_ghost = matrix + local_number;
    if (top != -1) {
        comm.send(high_ghost - width, width, MPI_DOUBLE, top, 0);
    }
    if (bottom != -1){
        comm.recv(low_ghost, width, MPI_DOUBLE, bottom, 0);
        comm.send(matrix, width, MPI_DOUBLE, bottom, 1);
    }
    if (top != -1){
        comm.recv(high_ghost, width, MPI_DOUBLE, top, 1);
    }
}

void data_exchange2(mpi::App& app, mpi::Communicator& comm, double* matrix){
    double* low_ghost = matrix - width;
    double* high_ghost = matrix + local_number;
    double* first_row = matrix;
    double* last_row = high_ghost - width;

    if (top == -1 && bottom == -1) return;
    if (top == -1){
        comm.recv(low_ghost, width, MPI_DOUBLE, bottom, 0);
        comm.send(first_row, width, MPI_DOUBLE, bottom, 1);
    } else if (bottom == -1){
        comm.send(last_row, width, MPI_DOUBLE, top, 0);
        comm.recv(high_ghost, width, MPI_DOUBLE, top, 1);
    } else {
        comm.sendrecv(last_row, width, MPI_DOUBLE, top, 0, low_ghost, width, MPI_DOUBLE, bottom, 0);
        comm.sendrecv(first_row, width, MPI_DOUBLE, bottom, 1, high_ghost, width, MPI_DOUBLE, top, 1);
    }
}

void data_exchange3(mpi::App& app, mpi::Communicator& comm, double* matrix){
    double* low_ghost = matrix - width;
    double* high_ghost = matrix + local_number;
    if (top != -1) {
        comm.bsend(high_ghost - width, width, MPI_DOUBLE, top, 0);
    }
    if (bottom != -1){
        comm.recv(low_ghost, width, MPI_DOUBLE, bottom, 0);
        comm.bsend(matrix, width, MPI_DOUBLE, bottom, 1);
    }
    if (top != -1){
        comm.recv(high_ghost, width, MPI_DOUBLE, top, 1);
    }
}

void data_exchange4(mpi::App& app, mpi::Communicator& comm, double* matrix){
    double* low_ghost = matrix - width;
    double* high_ghost = matrix + local_number;
    MPI_Request requests[4];
    int count = 0;
    if (top != -1) {
        requests[count++] = comm.isend(high_ghost - width, width, MPI_DOUBLE, top, 0);
    }
    if (bottom != -1){
        requests[count++] = comm.irecv(low_ghost, width, MPI_DOUBLE, bottom, 0);
        requests[count++] = comm.isend(matrix, width, MPI_DOUBLE, bottom, 1);
    }
    if (top != -1){
        requests[count++] = comm.irecv(high_ghost, width, MPI_DOUBLE, top, 1);
    }
    comm.waitall(count, requests);
}

void solve(double* output, const double* input, const double* f){
    for (int i=0; i<local_number; i++){
        int x = get_abscissa(i);
        int y = get_ordinate(i);
        if (x == 0 || y == 0 || x == width-1 || y == height-1){
            output[i] = 0;
        } else {
            // output[i] = 0.25 * (input[i-1] + input[i+1] + input[i-width] + input[i+width] - f[i]);
            output[i] = 0.25 * (input[i-1]+input[i+1]+input[i-width]+input[i+width]-f[i]);
        }
    }
}

double difference(mpi::Communicator& comm, double* vector1, double* vector2){
    double local_sum = 0;
    for (int i=0; i < local_number; i++){
        local_sum += std::fabs(vector1[i] - vector2[i]);
    }
    double sum;
    comm.allreduce(&local_sum, &sum, 1, MPI_DOUBLE, MPI_SUM);
    sum /= width*height;
    return sum;
}

int main(int argc, char* argv[]){

    /* MPI initialization */
    mpi::App app(&argc, &argv);
    int numprocs = app.getAppCommunicator().getProcessorNumber();
    bool no_cycle = false;
    mpi::CartesianCommunicator cart = app.getAppCommunicator().createCartesianTopology(1, &numprocs, &no_cycle, false);
    {
        mpi::CartesianCommunicator::NeighborInfo info = cart.getNeighbors(0, mpi::CartesianCommunicator::upward);
        bottom = info.source;
        top = info.destination;
    }
    coord = cart.getCoordinates()[0];

    /* Data initialization */
    compute_local_indices(coord, numprocs);
    double* f = new double[local_number];
    fill(f);
    double* u = new double[local_number + 2*width];
    double* u_new = new double[local_number + 2*width];
    double* u_work = u + width;
    double* u_new_work = u_new + width;
    fill(u_work, true);
    fill(u, true);
    fill(u_work+width, true);
    fill(u_new_work, true);
    fill(u_new, true);
    fill(u_new+width, true);


    const int max_iterations = 100000;
    // const int max_iterations = 2;

    cart.attachBuffer<double>(2*width);
    for (int i=0; i < max_iterations; i++){
        // data_exchange1(app, cart, u_work);
        // data_exchange2(app, cart, u_work);
        // data_exchange3(app, cart, u_work);
        data_exchange4(app, cart, u_work);

        solve(u_new_work, u_work, f);

        double* tmp;
        tmp = u_new_work;
        u_new_work = u_work;
        u_work = tmp;

        tmp = u_new;
        u_new = u;
        u = tmp;

        double error = difference(cart, u_new_work, u_work);
        if (i%100 == 0) {
            app << "Iteration = " << i << " Error = " << error << "\n";
        }

        if (error < 1e-6){
            break;
        }
    }
    cart.detachBuffer<double>();

    print(cart, u_work);


    /* Deleting all undeleted data */
    delete [] f;
    delete [] u;
}