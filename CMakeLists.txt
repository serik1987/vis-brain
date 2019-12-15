cmake_minimum_required(VERSION 3.14)
project(vis-brain)

set(CMAKE_CXX_STANDARD 17)

add_executable(vis-brain main.cpp  mpi/Group.cpp mpi/Communicator.cpp mpi/CartesianCommunicator.cpp mpi/Datatype.cpp
        mpi/Intercommunicator.cpp mpi/GraphCommunicator.cpp mpi/AbstractGraphItem.cpp mpi/Graph.cpp mpi/GraphItem.cpp
        mpi/App.cpp sys/system.cpp mpi/exceptions.cpp mpi/File.cpp mpi/Info.cpp param/Engine.cpp sys/auxiliary.cpp
        param/Object.cpp Application.cpp sys/Folder.cpp log/Logger.h log/UserLogger.cpp log/Logger.cpp
        log/SystemLogger.cpp log/Engine.cpp data/Matrix.cpp data/LocalMatrix.cpp data/ContiguousMatrix.cpp
        data/LuDecomposer.cpp data/reader/Reader.cpp data/reader/Saver.cpp data/reader/BinReader.cpp
        data/reader/Loader.cpp data/reader/PngReader.cpp data/reader/ExternalSaver.cpp data/Interpolator.cpp
        data/stream/Stream.cpp data/stream/BinStream.cpp data/graph/ColorAxis.cpp data/stream/ExternalStream.cpp
        data/noise/NoiseEngine.cpp data/noise/noise.cpp processors/Processor.cpp processors/Equation.cpp
        stimuli/Stimulus.cpp stimuli/StationaryStimulus.cpp stimuli/StationaryGrating.cpp param/Loadable.cpp
        stimuli/StationaryGaborGrating.cpp stimuli/StationaryRectangularGrating.cpp stimuli/StationaryDotStimulus.cpp
        stimuli/StationaryBarStimulus.cpp stimuli/StationaryReaderStimulus.cpp stimuli/StepStimulusParameters.cpp
        stimuli/GratingStimulusParameters.cpp stimuli/MovingStimulus.cpp stimuli/MovingGrating.cpp
        stimuli/MovingGaborGrating.cpp stimuli/MovingRectangularGrating.cpp stimuli/BarStimulusParameters.cpp
        stimuli/MovingBarStimulus.cpp stimuli/DotStimulusParameters.cpp stimuli/MovingDotStimulus.cpp
        stimuli/StreamStimulus.cpp sys/ExternalMechanism.cpp stimuli/ExternalStimulus.cpp stimuli/ComplexStimulus.cpp
        stimuli/WeightedStimulus.cpp stimuli/BoundedStimulus.cpp stimuli/SequenceStimulus.cpp
        models/abstract/glm/StimulusSaturation.cpp models/abstract/glm/NoStimulusSaturation.cpp
        models/abstract/glm/GlmLayer.cpp models/abstract/glm/BrokenLineStimulusSaturation.cpp
        models/abstract/glm/HalfSigmoidStimulusSaturation.cpp processors/SingleOde.cpp
        models/abstract/glm/TemporalKernel.cpp models/abstract/glm/OdeTemporalKernel.cpp
        models/abstract/glm/OdeTemporalKernel.h methods/ExplicitEuler.cpp methods/ExplicitRecountEuler.cpp
        methods/KhoinMethod.cpp methods/ExplicitRungeKutta.cpp models/abstract/glm/SpatialKernel.cpp
        models/abstract/glm/GaussianSpatialKernel.cpp models/abstract/glm/DogFilter.cpp processors/State.cpp
        models/AbstractNetwork.cpp models/Layer.cpp models/Brain.cpp models/Network.cpp
        models/abstract/AbstractModel.cpp methods/EqualDistributor.cpp stimuli/StimulusBuilder.cpp jobs/Job.cpp
        jobs/JobBuilder.cpp jobs/SingleRunJob.cpp methods/MethodBuilder.cpp methods/DistributorBuilder.cpp
        analyzers/Analyzer.cpp analyzers/VsdAnalyzer.cpp analyzers/AnalysisBuilder.cpp analyzers/PrimaryAnalyzer.cpp analyzers/PrimaryAnalyzer.h analyzers/PrimaryVsdAnalyzer.cpp analyzers/PrimaryVsdAnalyzer.h sys/security.cpp sys/security.h analyzers/SecondaryAnalyzer.cpp analyzers/SecondaryAnalyzer.h analyzers/SecondaryVsdAnalyzer.cpp analyzers/SecondaryVsdAnalyzer.h analyzers/VsdWriter.cpp analyzers/VsdWriter.h)
add_library(stimulus-test-library SHARED stimuli/test_library.cpp)

target_link_libraries(vis-brain pthread png /usr/local/lib/libv8_monolith.a dl)

# include_directories(/home/serik1987/v8/v8/include)
include_directories(/usr/local/include/v8)

# These lines were added for compatibility with MS MPI
# include_directories("C:\\Program Files (x86)\\Microsoft SDKs\\MPI\\Include")
# link_directories("C:\\Program Files (x86)\\Microsoft SDKs\\MPI\\Lib\\x64")
# target_link_libraries(mpi2 "C:\\Program Files (x86)\\Microsoft SDKs\\MPI\\Lib\\x64\\msmpi.lib")