//
// Created by serik1987 on 21.09.19.
//

#include "data/noise/NoiseEngine.h"



void test_main(){
    using namespace std;

    data::noise::NoiseEngine& engine = Application::getInstance().getNoiseEngine();
    mpi::Communicator& comm = Application::getInstance().getAppCommunicator();

    logging::progress(0, 1, "Test of random number engine");
    {
        unsigned long min_value = engine.min();
        unsigned long max_value = engine.max();

        vector<unsigned long> numbers(10);
        for (auto& x: numbers){
            x = engine();
        }

        logging::enter();
        logging::debug("Minimum value = " + to_string(min_value));
        logging::debug("Maximum value = " + to_string(max_value));
        logging::debug("Random numbers within the given range are listed below:");
        for (auto x: numbers){
            logging::debug(to_string(x));
        }
        logging::debug("");
        logging::exit();
    }

    uniform_int_distribution<int> distr(1, 6);

    logging::progress(1, 1);

}