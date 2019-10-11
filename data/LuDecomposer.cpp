//
// (C) Sergei Kozhukhov, 2019
// (C) the Institute of Higher Nervous Activity and Neurophysiology, Russian Academy of Sciences, 2019
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

    ContiguousMatrix LuDecomposer::getLowerTriangle() const{
        ContiguousMatrix Result(comm, N, N, A.getWidthUm(), A.getHeightUm());
        ContiguousMatrix::Iterator result(Result, 0);

        for (int i=0; i < N; ++i){
            for (int j=0; j < i; ++j){
                result.val(i, j) = l(i, j);
            }
            result.val(i, i) = 1.0;
            for (int j = i+1; j < N; ++j){
                result.val(i, j) = 0.0;
            }
        }

        return Result;
    }

    ContiguousMatrix LuDecomposer::getUpperTriangle() const{
        ContiguousMatrix Result(comm, N, N, A.getWidthUm(), A.getHeightUm());
        ContiguousMatrix::Iterator result(Result, 0);

        for (int i=0; i < N; ++i){
            for (int j=0; j < i; ++j) {
                result.val(i, j) = 0.0;
            }
            for (int j=i; j < N; ++j){
                result.val(i, j) = u(i, j);
            }
        }

        return Result;
    }

    ContiguousMatrix LuDecomposer::getPermutationMatrix() const {
        ContiguousMatrix Result(comm, N, N, A.getWidthUm(), A.getHeightUm());
        ContiguousMatrix::Iterator result(Result, 0);

        for (int i=0; i < N; ++i){
            for (int j=0; j < N; ++j){
                result.val(i, j) = (double)(P[i] == j);
            }
        }

        return Result;
    }

    void LuDecomposer::solve(data::ContiguousMatrix &x, const data::ContiguousMatrix &b, int root, double *buf) const {
        if (comm.getRank() == root){
            if (b.getHeight() != N || b.getWidth() != 1 || x.getHeight() != N || x.getWidth() != 1){
                throw matrix_dimensions_mismatch();
            }
            bool buffer_not_created = false;
            if (buf == nullptr){
                buf = new double[N];
                buffer_not_created = true;
            }
            solve_lowerTriangle(buf, b);
            solve_upperTriangle(x, buf);
            if (buffer_not_created){
                delete [] buf;
            }

        }
    }

    void LuDecomposer::solve_lowerTriangle(double* buf, const ContiguousMatrix& b) const {
        ContiguousMatrix::ConstantIterator it(b, 0);
        for (int i=0; i < N; ++i){
            buf[i] = it[P[i]];
            for (int j = i-1; j >=0; --j){
                buf[i] -= l(i, j) * buf[j];
            }
        }
    }

    void LuDecomposer::solve_upperTriangle(data::ContiguousMatrix &x, const double *buf) const {
        ContiguousMatrix::Iterator x_it(x, 0);
        for (int i = N-1; i >= 0; --i){
            x_it[i] = buf[i];
            for (int j = i+1; j < N; ++j){
                x_it[i] -= u(i, j) * x_it[j];
            }
            x_it[i] /= u(i, i);
        }
    }

    void LuDecomposer::divide(data::ContiguousMatrix &X, const data::ContiguousMatrix &B, double *buffer_local,
                              double *result_local, double *external_result) {
        divide_checkMatrices(X, B);
        divide_init(B);
        divide_initBuffers(B, buffer_local, result_local, external_result);
        divide_lowerTriangle(B);
        divide_upperTriangle();
        divide_gather(resultLocal, result);
        divide_setResult(X, result);
        divide_clearBuffers();
    }

    void LuDecomposer::inverse(data::ContiguousMatrix &X, double *buffer_local, double *result_local,
            double *external_result) {
        inverse_checkMatrix(X);
        divide_init(X);
        divide_initBuffers(X, buffer_local, result_local, external_result);
        inverse_lowerTriangle();
        divide_upperTriangle();
        divide_gather(resultLocal, result);
        divide_setResult(X, result);
        divide_clearBuffers();
    }

    inline void LuDecomposer::divide_checkMatrices(const data::ContiguousMatrix &X, const ContiguousMatrix &B) const {
        if (X.getHeight() != B.getHeight() || X.getHeight() != A.getHeight()){
            throw matrix_dimensions_mismatch();
        }
        if (X.getWidth() != B.getWidth()){
            throw matrix_dimensions_mismatch();
        }
    }

    inline void LuDecomposer::inverse_checkMatrix(const data::ContiguousMatrix &X) const {
        if (X.getHeight() != A.getHeight() || X.getWidth() != A.getWidth()){
            throw matrix_dimensions_mismatch();
        }
    }

    inline void LuDecomposer::divide_init(const data::ContiguousMatrix &B) {
        M = B.getWidth();
        localJob = M/n + (M%n != 0);
        totalJob = localJob * n;
        jStart = r * localJob;
        jFinish = jStart + localJob;
        if (jFinish > M) jFinish = M;
    }

    inline double* LuDecomposer::divide_createBufferLocal(const data::ContiguousMatrix &B, bool initialized) {
        if (!initialized){
            divide_init(B);
        }
        return new double[N*localJob];
    }

    inline double* LuDecomposer::divide_createResultLocal(const data::ContiguousMatrix &B, bool initialized) {
        if (!initialized){
            divide_init(B);
        }
        return new double[N*localJob];
    }

    inline double* LuDecomposer::divide_createResult(const data::ContiguousMatrix &B, bool initialized) {
        if (!initialized){
            divide_init(B);
        }
        return new double[N*totalJob];
    }


    inline void LuDecomposer::divide_initBuffers(const data::ContiguousMatrix &B, double *buffer_local,
            double *result_local, double *external_result) {
        if (buffer_local == nullptr){
            bufferLocal = divide_createBufferLocal(B, true);
            bufferLocalInitialized = true;
        } else {
            bufferLocal = buffer_local;
            bufferLocalInitialized = false;
        }

        if (result_local == nullptr){
            resultLocal = divide_createResultLocal(B, true);
            resultLocalInitialized = true;
        } else {
            resultLocal = result_local;
            resultLocalInitialized = false;
        }

        if (external_result == nullptr){
            result = divide_createResult(B, true);
            resultInitialized = true;
        } else {
            result = external_result;
            resultInitialized = false;
        }
    }

    inline void LuDecomposer::divide_clearBuffers() {
        if (bufferLocalInitialized){
            delete [] bufferLocal;
        }
        if (resultLocalInitialized){
            delete [] resultLocal;
        }
        if (resultInitialized){
            delete [] result;
        }
    }

    inline void LuDecomposer::divide_lowerTriangle(const data::ContiguousMatrix &B) {
        ContiguousMatrix::ConstantIterator b(B, 0);
        for (int j = jStart, localCol = 0; j < jFinish; ++j, ++localCol){
            for (int i=0; i < N; ++i){
                double& value = bufferLocal[i*localJob + localCol];
                value = b.val(P[i], j);
                for (int k=0; k < i; ++k){
                    value -= l(i, k) * bufferLocal[k*localJob + localCol];
                }
            }
        }
    }

    inline void LuDecomposer::inverse_lowerTriangle() {
        for (int j = jStart, localCol = 0; j < jFinish; ++j, ++localCol){
            for (int i=0; i < N; ++i){
                double& value = bufferLocal[i*localJob + localCol];
                value = (double)(P[i] == j);
                for (int k = 0; k < i; ++k){
                    value -= l(i, k) * bufferLocal[k*localJob + localCol];
                }
            }
        }
    }

    inline void LuDecomposer::divide_upperTriangle() {
        for(int j=jStart, localJ = 0; j < jFinish; ++j, ++localJ){
            for (int i = N-1; i >= 0; --i){
                double& value = resultLocal[i*localJob + localJ];
                value = bufferLocal[i*localJob + localJ];
                for (int k = i+1; k < N; ++k){
                    value -= u(i, k) * resultLocal[k*localJob + localJ];
                }
                value /= u(i, i);
            }
        }
    }

    inline void LuDecomposer::divide_gather(const double *localBuffer, double *buffer) {
        for (int i=0; i < N; ++i) {
            comm.allGather(localBuffer, localJob, MPI_DOUBLE, buffer, localJob, MPI_DOUBLE);
            localBuffer += localJob;
            buffer += totalJob;
        }
    }

    inline void LuDecomposer::divide_setResult(data::ContiguousMatrix &X, double *result) {
        ContiguousMatrix::Iterator x(X, 0);
        for (int i=0; i < N; ++i){
            double* px = &(*x);
            memcpy(px, result, M * sizeof(double));
            x += M;
            result += totalJob;
        }
    }


}