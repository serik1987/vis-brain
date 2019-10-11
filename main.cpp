#include <unistd.h>
#include <exception>
#include "Application.h"
#include "exceptions.h"

#include "log/Engine.h"

int main(int argc, char* argv[]) {


    Application application(&argc, &argv);
    try {
        application.loadAllParameters();
        if ((application.getAppCommunicator().getRank() == 0 || application.isApplicationReady()) &&
        !application.isGui()){
            if (application.getCmd().empty()) {
                application.simulate();
            } else {
                std::cerr << "\033[33m\033[1mProgram was launched OK but the number of started processes is not"
                             " the same as given in the arguments. The application will be restarted\033[0m\n";
                application.clearEngines();
                system(application.getCmd().c_str());
            }
        }
    } catch (std::exception& e){
        if (application.getAppCommunicator().getRank() == 0) {
            std::cerr << "\033[31m\033[1mFAILED\nFATAL ERROR: \033[0m";
            std::cerr << e.what() << std::endl;
        }
    }

    return 0;
}