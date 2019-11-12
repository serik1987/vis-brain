//
// Created by serik1987 on 21.09.19.
//

#include "stimuli/StationaryStimulus.h"
#include "stimuli/StationaryGrating.h"
#include "data/stream/BinStream.h"

void test_main(){
    using namespace std;

    Application& app = Application::getInstance();
    mpi::Communicator& comm = app.getAppCommunicator();
    stim::Stimulus& stim = app.getStimulus();
    auto* stim2 = dynamic_cast<stim::StationaryStimulus*>(&stim);
    auto* stim3 = dynamic_cast<stim::StationaryGrating*>(&stim);
    stim.initialize();
    data::stream::BinStream stream(&stim.getOutput(), "output.bin", data::stream::Stream::Write, 100.0);

    logging::enter();
    logging::debug("grid x: " + to_string(stim.getGridX()));
    logging::debug("grid y: " + to_string(stim.getGridY()));
    logging::debug("size x: " + to_string(stim.getSizeX()));
    logging::debug("size y: " + to_string(stim.getSizeY()));
    logging::debug("luminance: " + to_string(stim.getLuminance()));
    logging::debug("contrast: " + to_string(stim.getContrast()));
    if (stim2 != nullptr){
        logging::debug("Prestimulus epoch: " + to_string(stim2->getPrestimulusEpoch()));
        logging::debug("Stimulus duration: " + to_string(stim2->getStimulusDuration()));
        logging::debug("Poststimulus epoch: " + to_string(stim2->getPoststimulusEpoch()));
    }
    if (stim3 != nullptr){
        logging::debug("Orientation: " + to_string(stim3->getOrientation()));
        logging::debug("Spatial frequency: " + to_string(stim3->getSpatialFrequency()));
        logging::debug("Spatial phase: " + to_string(stim3->getSpatialPhase()));
    }
    logging::debug("");
    logging::exit();

    double time;
    for (int i=0; time < stim.getRecordLength(); i++){
        time = 10.0*i;
        stim.update(time);
        stream.write(&stim.getOutput());
    }

    logging::progress(1, 1);
}