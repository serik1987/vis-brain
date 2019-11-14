//
// Created by serik1987 on 13.11.2019.
//

#ifndef MPI2_MOVINGBARSTIMULUS_H
#define MPI2_MOVINGBARSTIMULUS_H

#include "MovingStimulus.h"
#include "BarStimulusParameters.h"


namespace stim {

    class MovingBarStimulus: public MovingStimulus, public BarStimulusParameters {
    private:
        double speed = 0.0;

    protected:
        [[nodiscard]] std::string getProcessorName() override { return "stim:MovingBarStimulus"; }
        void loadMovingStimulusParameters(const param::Object& source) override;
        void broadcastMovingStimulusParameters() override;
        void setMovingStimulusParameter(const std::string& name, const void* pvalue) override;
        void updateMovingStimulus(double relativeTime) override;

    public:
        explicit MovingBarStimulus(mpi::Communicator& comm): MovingStimulus(comm) {};

        /**
         *
         * @return current stimulus speed in degrees per second
         */
        [[nodiscard]] double getSpeed() const { return speed; }

        /**
         * Sets the stimulus speed
         *
         * @param value stimulus speed in dps
         */
        void setSpeed(double value){
            speed = value;
        }
    };

}


#endif //MPI2_MOVINGBARSTIMULUS_H
