//
// Created by serik1987 on 15.10.2019.
//

#include "Stream.h"

namespace data::stream{

    Stream::Stream(data::Matrix *matrix, const std::string &filename, data::stream::Stream::StreamMode mode,
                   double frameRate, bool autoopen): comm(matrix->getCommunicator()) {
        this->matrix = matrix;
        this->filename = Application::getInstance().getOutputFolder() + "/" + filename;
        this->mode = mode;
        this->sampleRate = frameRate;
        this->autoopen = autoopen;
        this->opened = false;
        this->frameNumber = 0;
    }

}