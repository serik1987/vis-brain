//
// Created by serik1987 on 11.11.2019.
//

#include "Stimulus.h"
#include "StationaryStimulus.h"
#include "MovingStimulus.h"
#include "ExternalStimulus.h"
#include "ComplexStimulus.h"
#include "../data/LocalMatrix.h"
#include "../log/output.h"
#include "../sys/security.h"
#include "../compile_options.h"

namespace stim{

    void Stimulus::loadParameterList(const param::Object &source) {
        using std::endl;
        std::stringstream ss;

        setGridX(source.getIntegerField("grid_x"));
        setGridY(source.getIntegerField(("grid_y")));
        ss << "Stimulus dimensions: " << getGridX() << "x" << getGridY() << " pixels\n";
        setSizeX(source.getFloatField("size_x"));
        setSizeY(source.getFloatField("size_y"));
        ss << "Stimulus size: " << getSizeX() << "x" << getSizeY() << " degrees\n";
        setLuminance(source.getFloatField("luminance"));
        ss << "Luminance: " << getLuminance() << endl;
        setContrast(source.getFloatField("contrast"));
        ss << "Contrast: " << getContrast();

        logging::info(ss.str());
        loadStimulusParameters(source);
    }

    void Stimulus::broadcastParameterList() {
        Application& app = Application::getInstance();
        app.broadcastInteger(gridX, 0);
        app.broadcastInteger(gridY, 0);
        app.broadcastDouble(sizeX, 0);
        app.broadcastDouble(sizeY, 0);
        app.broadcastDouble(luminance, 0);
        app.broadcastDouble(contrast, 0);
        broadcastStimulusParameters();
    }

    void Stimulus::setParameter(const std::string &name, const void *pvalue) {
        if (name == "grid_x") {
            setGridX(*(int*)(pvalue));
        } else if (name == "grid_y") {
            setGridY(*(int*)(pvalue));
        } else if (name == "size_x") {
            setSizeX(*(int*)(pvalue));
        } else if (name == "size_y") {
            setSizeY(*(int*)(pvalue));
        } else if (name == "luminance") {
            setLuminance(*(double*)(pvalue));
        } else if (name == "contrast") {
            setContrast(*(double*)(pvalue));
        } else {
            setStimulusParameter(name, pvalue);
        }
    }

    void Stimulus::initialize(){
        output = new data::LocalMatrix(getCommunicator(), getGridX(), getGridY(),
                getSizeX(), getSizeY());
        initializeStimulus();
    }

    Stimulus* Stimulus::createStimulus(mpi::Communicator &comm, const std::string &mechanism) {
        using std::string;
        Stimulus* stimulus = nullptr;

        int delimiter = mechanism.find('.');
        string major_name = mechanism.substr(0, delimiter);
        string minor_name = mechanism.substr(delimiter+1);
        if (delimiter == string::npos){
            minor_name = "";
        }

        if (major_name == "stationary") {
            stimulus = StationaryStimulus::createStationaryStimulus(comm, minor_name);
        } else if (major_name == "moving") {
            stimulus = MovingStimulus::createMovingStimulus(comm, minor_name);
        } else if (major_name == "external") {
#if SERVER_BUILD==1
            sys::security_check("mechanism", minor_name);
#endif
            std::string lib_name = Processor::lookExternalMechanism("stimuli", minor_name);
            logging::info("External stimulus was selected. Mechanism name: " + minor_name);
            stimulus = new ExternalStimulus(comm, lib_name);
        } else if (major_name == "complex") {
            stimulus = ComplexStimulus::createComplexStimulus(comm, minor_name);
        } else {
            throw param::UnknownMechanism("stimulus:"+mechanism);
        }

        return stimulus;
    }


}
