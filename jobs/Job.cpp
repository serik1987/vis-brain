//
// Created by serik1987 on 30.11.2019.
//

#include "Job.h"
#include "SingleRunJob.h"
#include "../log/output.h"
#include "../analyzers/AnalysisBuilder.h"

namespace job{

    void Job::loadParameterList(const param::Object &source) {
        logging::info("Single-run job");
        setOutputFilePrefix(source.getStringField("output_file_prefix"));
        logging::info("Output file prefix: " + getOutputFilePrefix());
        loadJobParameters(source);
        createAnalyzers(source);
    }

    void Job::broadcastParameterList() {
        Application::getInstance().broadcastString(output_file_prefix, 0);
        broadcastJobParameters();
        broadcastAnalyzers();
    }

    Job *Job::createJob(mpi::Communicator &comm, const std::string &job_type) {
        Job* job = nullptr;

        if (job_type == "single-run"){
            job = new SingleRunJob(comm);
        }

        return job;
    }

    void Job::createAnalyzers(const param::Object& source) {
        param::Object analyzers = source.getObjectField("analysis");
        for (auto it = analyzers.begin(); it != analyzers.end(); ++it){
            auto analyzer_parameters = analyzers.getObjectField(*it);
            auto mechanism = analyzer_parameters.getStringField("mechanism");
            analysis_mechanisms.push_back(mechanism);
            auto* proc = equ::Processor::createProcessor(comm, mechanism);
            auto* analyzer = dynamic_cast<analysis::Analyzer*>(proc);
            if (analyzer == nullptr){
                throw analysis::AnalysisBuilder::incorrect_analyzer();
            }
            analysis_list.push_back(analyzer);
            analysis_names.push_back(*it);
            analysis_map[*it] = analyzer;
            analyzer->loadParameters(analyzer_parameters);
        }
    }

    void Job::broadcastAnalyzers() {
        auto& app = Application::getInstance();
        int analysis_number = analysis_mechanisms.size();
        app.broadcastInteger(analysis_number, 0);
        bool need_to_create = app.getAppCommunicator().getRank() != 0;
        auto mechanisms_it =analysis_mechanisms.begin();
        auto analyzer_it = analysis_list.begin();
        auto name_it = analysis_names.begin();

        for (int i=0; i < analysis_number; ++i){
            std::string mechanism;
            std::string name;
            if (!need_to_create){
                mechanism = *mechanisms_it;
                ++mechanisms_it;
                name = *name_it;
                ++name_it;
            }
            app.broadcastString(mechanism, 0);
            app.broadcastString(name, 0);
            analysis::Analyzer* analyzer;
            if (need_to_create){
                analysis_mechanisms.push_back(mechanism);
                analysis_names.push_back(name);
                equ::Processor* proc = equ::Processor::createProcessor(getJobCommunicator(), mechanism);
                analyzer = dynamic_cast<analysis::Analyzer*>(proc);
            } else {
                analyzer = *analyzer_it;
                ++analyzer_it;
            }
            analyzer->broadcastParameters();
            if (need_to_create){
                analysis_list.push_back(analyzer);
                analysis_map[name] = analyzer;
            }
        }
    }

    void Job::initializeAnalyzers() {
        for (auto panalyzer: analysis_list){
            panalyzer->initialize();
        }
    }

    void Job::updateAnalyzers(double time) {
        for (auto panalyzer: analysis_list){
            panalyzer->update(time);
        }
    }

    void Job::finalizeAnalyzers() {
        for (auto panalyzer: analysis_list){
            panalyzer->finalize();
        }
    }

    Job::~Job() {
        for (auto panalyzer: analysis_list){
            delete panalyzer;
        }
    }
}