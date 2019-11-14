//
// Created by serik1987 on 14.11.2019.
//

#ifndef MPI2_MOVINGDOTSTIMULUS_H
#define MPI2_MOVINGDOTSTIMULUS_H

#include "MovingStimulus.h"
#include "DotStimulusParameters.h"

namespace stim {

    class MovingDotStimulus: public MovingStimulus, public DotStimulusParameters {
    private:
        double velocityX = 0.0;
        double velocityY = 0.0;

    protected:
        std::string getProcessorName() override { return "stim::MovingDotStimulus"; }
        void loadMovingStimulusParameters(const param::Object& source) override;
        void broadcastMovingStimulusParameters() override;
        void setMovingStimulusParameter(const std::string& name, const void* pvalue) override;
        void updateMovingStimulus(double relativeTime) override;

    public:
        explicit MovingDotStimulus(mpi::Communicator& comm): MovingStimulus(comm) {};

        /**
         *
         * @return velocity on abscissa in dps
         */
        [[nodiscard]] double getVelocityX() const { return velocityX; }

        /**
         *
         * @return velocity on ordinate in dps
         */
        [[nodiscard]] double getVelocityY() const { return velocityY; }

        /**
         * Sets projection of stimulus velocity on abscissa
         *
         * @param value velocity projection on abscissa in dps
         */
        void setVelocityX(double value) { velocityX = value; }

        /**
         * Sets projection of stimulus velocity on ordinate
         *
         * @param value velocity projection on ordinate in dps
         */
        void setVelocityY(double value) { velocityY = value; }
    };

}


#endif //MPI2_MOVINGDOTSTIMULUS_H
