#include <unistd.h>
#include <exception>
#include "Application.h"
#include "exceptions.h"

void test(){
    throw ApplicationAlreadyExists();
}

int main(int argc, char* argv[]) {
    /*
    try {
        Application application(&argc, &argv);
        std::cerr << "execution ok\n";
        if ((application.getAppCommunicator().getRank() == 0 || application.isApplicationReady()) &&
            !application.isGui()) {
            if (application.getCmd().empty()) {
                application << "CONTINUE...\n";
            } else {
                std::cerr << "\033[33m\033[1mProgram was launched OK but the number of started processes is not"
                             " the same as given in the arguments. The application will be restarted\033[0m\n";
                system(application.getCmd().c_str());
            }
        }
    } catch (std::exception& e){
        std::cerr << "Program execution was terminated in the normal way due to exception\n";
        std::cerr << e.what() << std::endl;
        return -1;
    }

     */

    Application application(&argc, &argv);
    try {
        application.loadAllParameters();
        if ((application.getAppCommunicator().getRank() == 0 || application.isApplicationReady()) &&
        !application.isGui()){
            if (application.getCmd().empty()) {
                application.getAppCommunicator().barrier();
                application.lock();
                application << "CONTINUE...\n";
                application.unlock();
            } else {
                std::cerr << "\033[33m\033[1mProgram was launched OK but the number of started processes is not"
                             " the same as given in the arguments. The application will be restarted\033[0m\n";
                system(application.getCmd().c_str());
            }
        }
    } catch (std::exception& e){
        std::cerr << "\033[31m\033[1mFAILED\nFATAL ERROR: \033[0m";
        std::cerr << e.what() << std::endl;
    }

    return 0;
}