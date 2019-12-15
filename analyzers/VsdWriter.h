//
// Created by serik1987 on 03.12.2019.
//

#ifndef MPI2_VSDWRITER_H
#define MPI2_VSDWRITER_H

#include "SecondaryVsdAnalyzer.h"
#include "../data/stream/BinStream.h"

namespace analysis {

    class VsdWriter: public SecondaryVsdAnalyzer {
    private:
        std::string filename_prefix;
        data::stream::BinStream* stream = nullptr;

    protected:
        std::string getProcessorName() override { return "analysis::VsdWriter"; }
        bool isOutputContiguous() override { return false;}
        void loadAnalysisParameters(const param::Object& source) override;
        void broadcastAnalysisParameters() override;
        void setAnalyzerParameters(const std::string& name, const void* pvalue) override;
        void initializeVsd() override;
        void finalizeProcessor(bool destruct = false) noexcept final;

    public:
        explicit VsdWriter(mpi::Communicator& comm): SecondaryVsdAnalyzer(comm), Analyzer(comm) {};

        ~VsdWriter() override {
            finalizeProcessor(true);
        }

        /**
         *
         * @return true if secondary analyzer can be attached to this analyzer
         */
        bool isInputAcceptable() override { return false; }

        /**
         * Runs at each iteration.
         * for stimuli: changes the stimulus state and prepares output matrix at a certain time
         * for equations: just prepar[[nodiscard]]es the output matrix, without changing the state
         * for ODEs: makes all routines after changing the main output matrix.
         *
         * @param time current timestamp in milliseconds
         */
        void update(double time) override;

        /**
         *
         * @return the filename prefix
         */
        [[nodiscard]] std::string getFilenamePrefix() { return filename_prefix; }

        /**
         * Sets the filename prefix
         *
         * @param value the prefix value
         */
        void setFilenamePrefix(const std::string& value) {
#if SERVER_BUILD==1
            sys::security_check("filename", value);
#endif
            filename_prefix = value;
        }
    };

}


#endif //MPI2_VSDWRITER_H
