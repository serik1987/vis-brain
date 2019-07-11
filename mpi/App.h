//
// Created by User on 30.06.2019.
//

#ifndef MPI2_APP_H
#define MPI2_APP_H


#include <iostream>
#include <sstream>
#include <string>
#include "mpi.h"
#include "exceptions.h"
#include "Communicator.h"

namespace mpi {

    /**
     * The general class that represents the application. Follow the following rules:
     * 1. The class instance shall be created. Moreover, its instance shall be the first variable declared in
     * your main(...) function.
     * 2. No code shall be executed before creating an instance of mpi::App
     * 3. You can create the only instance of this class.
     *
     */
    class App {
    private:
        static bool initialized;
        Communicator comm;
        double startTime;
        double localTime;
        double localDifference;

        static App* instance;

        std::ostringstream aux_out;

        void printFormattedString(const std::string& x){
            time();
            std::ios_base::fmtflags mode = out.setf(std::ios_base::fixed, std::ios_base::floatfield);
            std::streamsize prec = out.precision(6);
            out << "[" << comm.getRank() << "/" << comm.getProcessorNumber() << " | " <<
                getLocalDifference() << "s | " << getGlobalDifference() << "s since start ] ";
            out.setf(mode, std::ios_base::floatfield);
            out.precision(prec);
            out << x;
        }

        void printRemainingString(){
            std::string buffer = aux_out.str();
            aux_out.str("");
            if (buffer.length()!=0 && comm.getRank() == 0) {
                printFormattedString(buffer);
                out << ": ";
            }
        }

    public:

        std::ostream& out; // The output stream that was used when you try app << x;
        std::istream& in; // The input stream for parameters, not for data


        /**
         * Creates the instance of the application
         *
         * @param argc pointer to argc argument in main(...) function
         * @param argv pointer to argv argument in main(...) function
         * @param base_output (optional) the output of the data
         * @param base_input (optional) the input of the data
         */
        App(int *argc, char ***argv, std::ostream& base_output = std::cout, std::istream& base_input = std::cin):
            out(base_output), in(base_input), comm(MPI_COMM_WORLD) {
            /* Constructor. argc, argv are pointers to parameters in main() function */
            /*
             * 1. Initializes the MPI englne.
             * 2. Starts the stopwatch.
             */
            if (!initialized) {
                int err_code = MPI_Init(argc, argv);
                if (err_code != MPI_SUCCESS){
                    throw_exception(err_code);
                }
                initialized = true;
                instance = this;
                startTime = localTime = MPI_Wtime();
            } else {
                throw already_initialized_exception();
            }
        }

        /**
         *
         * @return an instance of this class if you lose it
         */
        static App* getInstance(){
            return instance;
        }

        App() = delete;

        App(const App &) = delete;

        App(App &&) = delete;

        App &operator=(const App &other) = delete;

        ~App() {
            comm.~Communicator();
            MPI_Finalize();
            exit(EXIT_SUCCESS);
        }


        /**
         * Returns the default communicator that contains all processes
         *
         * @return the default communicator
         */
        Communicator& getAppCommunicator(){
            return comm;
        }


        /**
         * Presses the stopwatch
         */
        void time(){
            double x = MPI_Wtime();
            localDifference = x - localTime;
            localTime = x;
        }

        /**
         * Returns the time interval (s) between the two previous pushes of the stopwatch
         *
         * @return the time interval in seconds
         */
        double getLocalDifference(){
            return localDifference;
        }

        /**
         *
         * @return Returns the time interval (s) between the previous push of the stopwatch and the MPI engine init
         */
        double getGlobalDifference(){
            return localTime - startTime;
        }




        /** Outputs the data */
        template<typename T> App& operator<<(const T& other){
            using std::string;
            aux_out << other;
            string buffer = aux_out.str();
            size_t pos = buffer.find('\n');
            if (pos != std::string::npos){
                printFormattedString(buffer.substr(0, pos));
                out << std::endl;
                aux_out.str(buffer.substr(pos+1));
            }
            return *this;
        }

        /** Inputs the data */
        template <typename T> App& operator>>(T& n){
            printRemainingString();
            if (comm.getRank() == 0){
                in >> n;
            }
            comm.broadcast(&n, 1, ScalarType<T>(), 0);
            time();
            return *this;
        }

        /**
         * Terminates the application immediately
         *
         * @param errcode - the code that will be thrown to the external medium
         */
        void abort(int errcode){
            MPI_Abort(*comm, errcode);
        }

    };
}


#endif //MPI2_APP_H
