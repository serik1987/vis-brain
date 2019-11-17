//
// Created by serik1987 on 21.09.19.
//

#include "stimuli/StationaryStimulus.h"
#include "stimuli/StationaryGrating.h"
#include "stimuli/StationaryDotStimulus.h"
#include "data/stream/BinStream.h"
#include "stimuli/StationaryBarStimulus.h"
#include "stimuli/StationaryReaderStimulus.h"
#include "stimuli/MovingStimulus.h"
#include "stimuli/MovingGrating.h"
#include "stimuli/MovingBarStimulus.h"
#include "stimuli/MovingDotStimulus.h"
#include "stimuli/StreamStimulus.h"
#include "stimuli/ExternalStimulus.h"
#include "stimuli/ComplexStimulus.h"
#include "stimuli/WeightedStimulus.h"
#include "stimuli/SequenceStimulus.h"

void test_main(){
    using namespace std;

    Application& app = Application::getInstance();
    mpi::Communicator& comm = app.getAppCommunicator();
    app.createStimulus(comm);
    stim::Stimulus& stim = app.getStimulus();
    auto* stim2 = dynamic_cast<stim::StationaryStimulus*>(&stim);
    auto* stim3 = dynamic_cast<stim::StationaryGrating*>(&stim);
    auto* stim4 = dynamic_cast<stim::StationaryDotStimulus*>(&stim);
    auto* stim5 = dynamic_cast<stim::StationaryBarStimulus*>(&stim);
    auto* stim6 = dynamic_cast<stim::StationaryReaderStimulus*>(&stim);
    auto* moving_stimulus = dynamic_cast<stim::MovingStimulus*>(&stim);
    auto* moving_grating = dynamic_cast<stim::MovingGrating*>(&stim);
    auto* moving_bar = dynamic_cast<stim::MovingBarStimulus*>(&stim);
    auto* moving_dot = dynamic_cast<stim::MovingDotStimulus*>(&stim);
    auto* stream_stimulus = dynamic_cast<stim::StreamStimulus*>(&stim);
    auto* external_stimulus = dynamic_cast<stim::ExternalStimulus*>(&stim);
    auto* complex_stimulus = dynamic_cast<stim::ComplexStimulus*>(&stim);
    auto* weighted_stimulus = dynamic_cast<stim::ComplexStimulus*>(&stim);
    auto* sequence_stimulus = dynamic_cast<stim::SequenceStimulus*>(&stim);

    logging::progress(0, 1, "Testing stimulus");

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
    if (stim4 != nullptr){
        logging::debug("X = " + to_string(stim4->getX()));
        logging::debug("Y = " + to_string(stim4->getY()));
        logging::debug("radius = " + to_string(stim4->getRadius()));
    }
    if (stim5 != nullptr){
        logging::debug("Bar length: " + to_string(stim5->getLength()));
        logging::debug("Bar width: " + to_string(stim5->getWidth()));
        logging::debug("X = " + to_string(stim5->getX()));
        logging::debug("Y = " + to_string(stim5->getY()));
        logging::debug("Bar orientation, rad: " + to_string(stim5->getOrientation()));
    }
    if (moving_stimulus != nullptr){
        logging::debug("Prestimulus epoch: " + to_string(moving_stimulus->getPrestimulusEpoch()));
        logging::debug("Stimulus duration: " + to_string(moving_stimulus->getStimulusDuration()));
        logging::debug("Poststimulus epoch: " + to_string(moving_stimulus->getPoststimulusEpoch()));
        logging::debug("Stimulus start: " + to_string(moving_stimulus->getStimulusStart()));
        logging::debug("Stimulus finish: " + to_string(moving_stimulus->getStimulusFinish()));
        logging::debug("Record length: " + to_string(moving_stimulus->getRecordLength()));
    }
    if (moving_grating != nullptr){
        logging::debug("Spatial frequency: " + to_string(moving_grating->getSpatialFrequency()));
        logging::debug("Statial phase: " + to_string(moving_grating->getSpatialPhase()));
        logging::debug("Orientation: " + to_string(moving_grating->getOrientation()));
        logging::debug("Temporal frequency: " +to_string(moving_grating->getTemporalFrequency()));
    }
    if (stim6 != nullptr){
        logging::debug("Filename: " + stim6->getFilename());
    }
    if (moving_bar != nullptr){
        logging::debug("Bar length: " + to_string(moving_bar->getLength()));
        logging::debug("Bar width: " + to_string(moving_bar->getWidth()));
        logging::debug("Bar orientation, rad: " + to_string(moving_bar->getOrientation()));
        logging::debug("x0 = " + to_string(moving_bar->getX()));
        logging::debug("y0 = " + to_string(moving_bar->getY()));
        logging::debug("Speed, dps: " + to_string(moving_bar->getSpeed()));
    }
    if (moving_dot != nullptr){
        logging::debug("X = " + to_string(moving_dot->getX()));
        logging::debug("Y = " + to_string(moving_dot->getY()));
        logging::debug("Dot radius, deg: " + to_string(moving_dot->getRadius()));
        logging::debug("Velocity projection on X, dps: " + to_string(moving_dot->getVelocityX()));
        logging::debug("Velocity projection on Y, dps: " + to_string(moving_dot->getVelocityY()));
    }
    if (stream_stimulus != nullptr){
        logging::debug("Filename: " + stream_stimulus->getFilename());
    }
    if (external_stimulus != nullptr){
        logging::debug("Total number of parameters: " + to_string(external_stimulus->getParameterNumber()));
        for (int i=0; i < external_stimulus->getParameterNumber(); i++){
            stringstream ss;
            const char* parname = external_stimulus->getParameterName(i);
            ss << parname << ": " << external_stimulus->getParameter(parname);
            logging::debug(ss.str());
        }
    }
    if (complex_stimulus != nullptr){
        cout << "PROCESSORS TREE FOR ALL PROCESSORS WITH RANK # " << comm.getRank() << ":\n";
        complex_stimulus->printAllProcessors(0, comm.getRank());
        logging::debug("Record length: " + to_string(complex_stimulus->getRecordLength()));
    }
    if (weighted_stimulus != nullptr){
        int idx = 0;
        for (auto it = weighted_stimulus->inputProcessorBegin(); it != weighted_stimulus->inputProcessorEnd(); ++it){
            auto* dot_stimulus = dynamic_cast<stim::StationaryDotStimulus*>(*it);
            if (dot_stimulus != nullptr){
                stringstream ss;
                ss << "STIMULUS # " << idx << ". X = " << dot_stimulus->getX() << ". Y = " << dot_stimulus->getY();
                logging::debug(ss.str());
            }
            idx ++;
        }
    }
    if (sequence_stimulus != nullptr){
        stringstream ss;
        ss << "Shuffling: " << sequence_stimulus->isShuffle() << endl;
        ss << "Number of repeats: " << sequence_stimulus->getRepeats() << endl;
        ss << "Name: " << sequence_stimulus->getName() << endl;
        logging::debug(ss.str());
    }
    logging::debug("");
    logging::exit();

    stim.initialize();
    data::stream::BinStream stream(&stim.getOutput(), "output.bin", data::stream::Stream::Write, 100.0);

    logging::enter();
    if (complex_stimulus != nullptr){
        logging::debug("Record length, after initialization, ms: " + std::to_string(complex_stimulus->getRecordLength()));
    }
    logging::exit();

    double time = 0.0;
    for (int i=0; time < stim.getRecordLength(); i++){
        stim.update(time);
        stream.write(&stim.getOutput());
        time = 10.0*i;
    }

    stim.finalize();

    logging::enter();
    if (complex_stimulus != nullptr){
        logging::debug("Record length, after finalization, ms: " + std::to_string(complex_stimulus->getRecordLength()));
    }
    logging::exit();

    logging::progress(1, 1);
}