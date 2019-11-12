//
// Created by serik1987 on 21.09.19.
//

#include "stimuli/StationaryGrating.h"
#include "data/reader/BinReader.h"
#include "data/stream/BinStream.h"

void test_main(){
    using namespace std;

    logging::progress(0, 1, "Test of parameter loading to the stimulus");

    Application& app = Application::getInstance();
    mpi::Communicator& comm = app.getAppCommunicator();
    stim::StationaryGrating grating(comm);

    if (comm.getRank() == 0) {
        param::Engine& engine = app.getParameterEngine();
        param::Object parameters = engine.getRoot().getObjectField("stimulus");
        grating.loadParameters(parameters);
    }
    grating.broadcastParameters();

    logging::enter();
    logging::debug(grating.getName());
    logging::debug("Stimulus grid on X: " + to_string(grating.getGridX()));
    logging::debug("Stimulus grid on Y: " + to_string(grating.getGridY()));
    logging::debug("Stimulus size on X: " + to_string(grating.getSizeX()));
    logging::debug("Stimulus size on Y: " + to_string(grating.getSizeY()));
    logging::debug("Luminance: " + to_string(grating.getLuminance()));
    logging::debug("Contrast: " + to_string(grating.getContrast()));
    logging::debug("Prestimulus epoch: " + to_string(grating.getPrestimulusEpoch()));
    logging::debug("Poststimulus epoch: " + to_string(grating.getPoststimulusEpoch()));
    logging::debug("Stimulus duration: " + to_string(grating.getStimulusDuration()));
    logging::debug("Stimulus start: " + to_string(grating.getStimulusStart()));
    logging::debug("Stimulus finish: " + to_string(grating.getStimulusFinish()));
    logging::debug("Record length: " + to_string(grating.getRecordLength()));
    logging::debug("Orientation: " + to_string(grating.getOrientation()));
    logging::debug("Spatial frequency: " + to_string(grating.getSpatialFrequency()));
    logging::debug("Spatial phase: " + to_string(grating.getSpatialPhase()));
    logging::debug("");
    logging::exit();

    grating.initialize();

    data::reader::BinReader reader("test-output.bin");
    reader.save(grating.getOutput());

    logging::progress(0, 1, "Saving the whole stimulus history");
    logging::enter();
    logging::debug(grating.getName());
    logging::exit();

    data::stream::BinStream stream(&grating.getOutput(), "output.bin", data::stream::Stream::Write, 100.0);
    {
        double time = 0.0;
        for (int i=0; time < grating.getRecordLength(); ++i){
            time = 10.0*i;
            grating.update(time);
            stream.write(&grating.getOutput());
        }
    }

    grating.finalize();

    logging::progress(1, 1);

}