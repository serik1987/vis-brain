//
// Created by serik1987 on 03.10.19.
//

#include "LuDecomposer.h"

namespace data {

    LuDecomposer::LuDecomposer(ContiguousMatrix& source, const std::string& userPrompt, int userInterval):
        A(source), comm(source.getCommunicator()), _a(source, 0), prompt(userPrompt), interval(userInterval),
        decomposingData(nullptr){

        if (A.getWidth() != A.getHeight()){
            throw square_matrix_required();
        }

        N = A.getWidth();
        n = comm.getProcessorNumber();
        r = comm.getRank();
        i = j = -1;
        decomposingData = new double[N*N];
        initializeP();
        lBest = new double[N*n];
        lBestLocal = new double[N];
        uLocal = lBestLocal; // Because these buffers will be used at different times
        uTotal = lBest; // Because these buffers will be used at different times
        uBest = new double[n];
        pBest = new int[n];

        if (prompt != ""){
            Application::getInstance().time();
            logging::progress(0, N, prompt);
            computationTime = 0.0;
            transmissionTime = 0.0;
        }

        decompose();
    }

    LuDecomposer::~LuDecomposer(){
        delete [] decomposingData;
        delete [] lBest;
        delete [] lBestLocal;
        delete [] uBest;
        delete [] pBest;
    }

    void LuDecomposer::initializeP() {
        P = new int[N];
        for (i=0; i < N; ++i){
            P[i] = i;
        }
    }

#if DEBUG==1
    void LuDecomposer::printDebugInformation(){
        double value;
        int row, col;

        using namespace std;
        logging::enter();
        if (r == 0){
            logging::debug("Decomposition results:");
            logging::debug("Total number of items in the matrix: " + to_string(N));
            logging::debug("Total number of processes: " + to_string(n));
            logging::debug("Rank of the current process: " + to_string(r));
            logging::debug("Current row: " + to_string(i));
            logging::debug("Current column: " + to_string(j));
            logging::debug("L matrix info:");
            logging::debug("(0, 0): " + to_string(L(0, 0)));
            logging::debug("(0, N-1): " + to_string(L(0, N-1)));
            logging::debug("(N-1, 0): " + to_string(L(N-1, 0)));
            logging::debug("(N-1, N-1): " + to_string(L(N-1, N-1)));
            getLmax(value, row, col);
            logging::debug("Max. value: " + to_string(value) + " at (" + to_string(row) + ", " + to_string(col) + ")");
            getLmin(value, row, col);
            logging::debug("Min. value: " + to_string(value) + " at (" + to_string(row) + ", " + to_string(col) + ")");
            logging::debug("filled cells: " + to_string(getLfilled()));
            logging::debug("U matrix info:");
            logging::debug("(0, 0): " + to_string(U(0, 0)));
            logging::debug("(0, N-1): " + to_string(U(0, N-1)));
            logging::debug("(N-1, 0): " + to_string(U(N-1, 0)));
            logging::debug("(N-1, N-1): " + to_string(U(N-1, N-1)));
            getUmax(value, row, col);
            logging::debug("Max. value: " + to_string(value) + " at (" + to_string(row) + ", " + to_string(col) + ")");
            getUmin(value, row, col);
            logging::debug("Min. value: " + to_string(value) + " at (" + to_string(row) + ", " + to_string(col) + ")");
            logging::debug("filled cells: " + to_string(getUfilled()));
            logging::debug("uBest:");
            for (int k = 0; k < n; ++k){
                logging::debug(to_string(uBest[k]));
            }
            logging::debug("pBest: ");
            for (int k = 0; k < n; ++k){
                logging::debug(to_string(pBest[k]));
            }
            logging::debug("lBest = uTotal = :");
            for (int k = 0; k < n; ++k){
                double* bestRow = lBest + N*k;
                std::string S = "";
                for (int j=0; j < 5; ++j){
                    S += to_string(bestRow[j]) + "\t";
                }
                S += "...";
                logging::debug(S);
            }
            logging::debug("");
        }
        logging::debug("Computation time: " + to_string(computationTime));
        logging::debug("Transmission time: " + to_string(transmissionTime));
        logging::debug("Total time: " + to_string(computationTime + transmissionTime));
        logging::debug("pStart = " + to_string(pStart));
        logging::debug("pFinish = " + to_string(pFinish));
        logging::debug("pConsequtive = " + to_string(pConsequtive));
        logging::debug("jStart = " + to_string(jStart));
        logging::debug("jFinish = " + to_string(jFinish));
        logging::debug("uBestLocal = " + to_string(uBestLocal));
        logging::debug("pBestLocal = " + to_string(pBestLocal));
        std::string S = "lBestLocal = uLocal = ";
        for (int j=0; j < 5; ++j){
            S += to_string(lBestLocal[j]) + "\t";
        }
        S += "...";
        logging::debug(S);
        logging::debug("");
        logging::exit();
    }

    void LuDecomposer::getUmax(double &value, int &row, int &col) {
        value = u(0, 0);
        row = col = 0;
        for (i=1; i < N; ++i){
            for (j=i; j < N; ++j){
                if (abs(u(i,j)) > abs(value)){
                    value = u(i,j);
                    row = i;
                    col = j;
                }
            }
        }
    }

    void LuDecomposer::getUmin(double &value, int &row, int &col) {
        value = u(0, 0);
        row = col = 0;
        for (int i=1; i < N; ++i){
            for (int j=i; j < N; ++j){
                if (abs(u(i, j)) < abs(value)){
                    value = u(i, j);
                    row = i;
                    col = j;
                }
            }
        }
    }

    void LuDecomposer::getLmax(double &value, int &row, int &col) {
        value = l(1, 0);
        row = 1, col = 0;
        for (i = 2; i < N; ++i){
            for (j = 0; j < i; ++j){
                if (abs(l(i, j)) > abs(value)){
                    value = l(i, j);
                    row = i;
                    col = j;
                }
            }
        }
    }

    void LuDecomposer::getLmin(double &value, int &row, int &col) {
        value = l(1, 0);
        row = 1, col = 0;
        for (i = 2; i < N; ++i){
            for (j = 0; j < i; ++j){
                if (abs(l(i, j)) < abs(value)){
                    value = l(i, j);
                    row = i;
                    col = j;
                }
            }
        }
    }

    int LuDecomposer::getLfilled() {
        int filled = 0;
        for (int i=0; i < N; ++i){
            for (int j = 0; j < i; ++j){
                if (l(i, j) != 0){
                    ++filled;
                }
            }
        }
        return filled;
    }

    int LuDecomposer::getUfilled() {
        int filled = 0;
        for (int i=0; i < N; ++i){
            for (int j = i; j < N; ++j){
                if (u(i, j) != 0){
                    ++filled;
                }
            }
        }
        return filled;
    }
#endif

    void LuDecomposer::decompose(){
        decomposeFirstRow();
        for (i = 1; i < N; ++i){
            computeLBestParallel();
            computeUConsequtive();
            if (i % interval == 0){
                logging::progress(i, N);
            }
            /*
            if (i == 300){
                break;
            }
             */
        }
    }

    void LuDecomposer::decomposeFirstRow() {
        double maxDiagonal;
        int row;
        i = 0;
        searchMaxDiagonal(maxDiagonal, row);
        chooseBest(maxDiagonal, row);
        initU();
    }

    void LuDecomposer::searchMaxDiagonal(double& value, int& row){
        value = 0.0;
        int load = ceil((double)N/n);
        int iStart = r * load;
        int iFinish = iStart + load;
        if (iFinish > N){
            iFinish = N;
        }
        for (int i = iStart; i < iFinish; ++i){
            if (abs(a(i,0)) > value){
                value = a(i, 0);
                row = i;
            }
        }
        checkComputationTime();
    }

    void LuDecomposer::chooseBest(double uBest_local, int pBest_local) {
        comm.allGather(&uBest_local, 1, MPI_DOUBLE, uBest, 1, MPI_DOUBLE);
        comm.allGather(&pBest_local, 1, MPI_INT, pBest, 1, MPI_INT);
        double uBestBest = 0.0;
        int pBestBest;
        for (int k=0; k < n; ++k){
            if (abs(uBest[k]) > uBestBest){
                uBestBest = uBest[k];
                pBestBest = pBest[k];
            }
        }
        P[i] = pBestBest;
        P[pBestBest] = i;
        checkTransmissionTime();
    }

    void LuDecomposer::checkComputationTime(){
        Application::getInstance().time();
        computationTime += Application::getInstance().getLocalDifference();
    }

    void LuDecomposer::checkTransmissionTime(){
        Application::getInstance().time();
        transmissionTime += Application::getInstance().getLocalDifference();
    }

    void LuDecomposer::initU(){
       for (j = i; j < N; ++j){
            u(i, j) = pa(i, j);
       }
       checkComputationTime();
    }

    void LuDecomposer::computeLBestParallel() {
        int total_load = N - i;
        int local_load = total_load / n + (total_load % n != 0);
        if (local_load == 0){
            pConsequtive = i;
            return;
        }
        pStart = i + r * local_load;
        pFinish = pStart + local_load;
        if (pFinish > N) pFinish = N;
        pConsequtive = i + n * local_load;
        uBestLocal = 0.0;
        for (p = pStart; p < pFinish; ++p){
            computeLConsequtive(p);
            computeUDiagonal(p);
            if (abs(u(i, i)) > uBestLocal){
                checkUBestLocal(p);
            }
        }
        checkComputationTime();
        chooseLBest();
    }

    void LuDecomposer::computeLConsequtive(int p0){
        for (j = 0; j < i; ++j){
            l(i, j) = pa(p0, j);
            for (int k=0; k < j; ++k){
                l(i, j) -= l(i, k) * u(k, j);
            }
            l(i, j) /= u(j, j);
        }
    }

    void LuDecomposer::computeUDiagonal(int p0){
        u(i, i) = pa(p0, i);
        for (int k=0; k < i; ++k){
            u(i, i) -= l(i, k) * u(k, i);
        }
    }

    void LuDecomposer::checkUBestLocal(int p0) {
        for (int k = 0; k < i; ++k){
            lBestLocal[k] = l(i, k);
        }
        uBestLocal = u(i, i);
        pBestLocal = p0;
    }

    void LuDecomposer::chooseLBest(){
        comm.allGather(lBestLocal, N, MPI_DOUBLE, lBest, N, MPI_DOUBLE);
        comm.allGather(&uBestLocal, 1, MPI_DOUBLE, uBest, 1, MPI_DOUBLE);
        comm.allGather(&pBestLocal, 1, MPI_INT, pBest, 1, MPI_INT);
        double uBestBest = 0.0;
        int k0;
        for (int k=0; k < n; ++k){
            if (abs(uBest[k]) > uBestBest){
                uBestBest = uBest[k];
                k0 = k;
            }
        }
        int pBestBest = pBest[k0];
        double* lBestBest = lBest + N * k0;
        int index = P[pBestBest];
        P[pBestBest] = P[i];
        P[i] = index;
        for (int j = 0; j < i; ++j){
            l(i, j) = lBestBest[j];
        }
        u(i, i) = uBestBest;
        checkTransmissionTime();
    }

    void LuDecomposer::computeUConsequtive(){
        int total_load = N - i - 1;
        int local_load = total_load / n + (total_load % n != 0);
        jStart = i + 1 + r * local_load;
        jFinish = jStart + local_load;
        if (jFinish > N) jFinish = N;
        int displ = 0;
        for (j = jStart; j < jFinish; ++j){
            uLocal[displ] = pa(i, j);
            for (int k=0; k < i; ++k){
                uLocal[displ] -= l(i, k) * u(k, j);
            }
            ++displ;
        }
        checkComputationTime();
        transmitU(local_load);
    }

    void LuDecomposer::transmitU(int local_load){
        comm.allGather(uLocal, local_load, MPI_DOUBLE, uTotal, local_load, MPI_DOUBLE);
        int displ = 0;
        for (j = i+1; j < N; ++j){
            u(i, j) = uTotal[displ];
            ++displ;
        }
        checkTransmissionTime();
    }


}